#include "rdKeyframe.h"

#include "Engine/rdroid.h"
#include "General/stdConffile.h"
#include "stdPlatform.h"
#include "jk.h"

void rdKeyframe_RegisterLoader(keyframeLoader_t loader)
{
    pKeyframeLoader = loader;
}

void rdKeyframe_RegisterUnloader(keyframeUnloader_t loader)
{
    pKeyframeUnloader = loader;
}

void rdKeyframe_NewEntry(rdKeyframe *keyframe)
{
    memset(keyframe, 0, sizeof(rdKeyframe));
    strncpy(keyframe->name, "UNKNOWN", 0x1Fu);
    keyframe->name[31] = 0;
}

rdKeyframe* rdKeyframe_Load(char *fname)
{
    rdKeyframe *keyframe;

    if (pKeyframeLoader)
        return (rdKeyframe*)pKeyframeLoader(fname);

    keyframe = (rdKeyframe*)rdroid_pHS->alloc(0x84u);
    if (!keyframe)
        return NULL;

    if (rdKeyframe_LoadEntry(fname, keyframe))
      return keyframe;

    // This was inlined
    rdKeyframe_FreeEntry(keyframe);
    
    return NULL;
}

int rdKeyframe_LoadEntry(char *key_fpath, rdKeyframe *keyframe)
{
    char *key_fname_only; // eax
    rdJoint *joints; // edi
    rdKeyframe *num_joints; // eax
    unsigned int num_markers_read; // edi
    rdMarkers *markers; // esi
    rdJoint *joint; // edi
    rdAnimEntry *v11; // eax
    rdAnimEntry *anim_entry; // esi
    int anim_entry_read; // ebp
    unsigned int num_markers; // [esp+10h] [ebp-68h]
    int node_idx; // [esp+14h] [ebp-64h]
    int anim_entry_cnt; // [esp+18h] [ebp-60h]
    unsigned int num_nodes; // [esp+1Ch] [ebp-5Ch]
    rdVector3 pos; // [esp+24h] [ebp-54h]
    rdVector3 orientation; // [esp+30h] [ebp-48h]
    rdVector3 vel; // [esp+3Ch] [ebp-3Ch]
    rdVector3 angVel; // [esp+48h] [ebp-30h]
    char entry_num; // [esp+54h] [ebp-24h]
    char mesh_name[32]; // [esp+58h] [ebp-20h]
    unsigned int nodes_read; // [esp+7Ch] [ebp+4h]

    _memset(keyframe, 0, sizeof(rdKeyframe));
    _strncpy(keyframe->name, "UNKNOWN", 0x1Fu);
    keyframe->name[31] = 0;
    key_fname_only = stdFileFromPath(key_fpath);
    _strncpy(keyframe->name, key_fname_only, 0x1Fu);
    keyframe->name[31] = 0;
    if (!stdConffile_OpenRead(key_fpath))
        goto read_fail;

    if (!stdConffile_ReadLine())
      goto read_fail;

    if (_sscanf(stdConffile_aLine, " section: %s", std_genBuffer) != 1)
      goto read_fail;

    if (!stdConffile_ReadLine())
      goto read_fail;

    if (_sscanf(stdConffile_aLine, " flags %d", &keyframe->flags) != 1)
      goto read_fail;

    if (!stdConffile_ReadLine())
      goto read_fail;

    if (_sscanf(stdConffile_aLine, " type %x", &keyframe->type) != 1)
      goto read_fail;

    if (!stdConffile_ReadLine())
      goto read_fail;

    if (_sscanf(stdConffile_aLine, " frames %d", &keyframe->numFrames) != 1)
      goto read_fail;

    if (!stdConffile_ReadLine())
      goto read_fail;

    if (_sscanf(stdConffile_aLine, " fps %f", &keyframe->fps) != 1)
      goto read_fail;

    if (!stdConffile_ReadLine())
      goto read_fail;

    if (_sscanf(stdConffile_aLine, " joints %d", &keyframe->numJoints) != 1)
      goto read_fail;

    joints = (rdJoint *)rdroid_pHS->alloc(sizeof(rdJoint) * keyframe->numJoints);
    keyframe->joints = joints;
    if (!joints)
      goto read_fail;

    _memset(joints, 0, sizeof(rdJoint) * keyframe->numJoints);
    keyframe->numJoints2 = keyframe->numJoints;

    if (!stdConffile_ReadLine() || _sscanf(stdConffile_aLine, " section: %s", std_genBuffer) != 1)
      goto read_fail;

    if (!_memcmp(std_genBuffer, "markers", 8u))
    {
      if (!stdConffile_ReadLine())
        goto read_fail;

      if (_sscanf(stdConffile_aLine, " markers %d", &num_markers) != 1)
        goto read_fail;

      if (num_markers > 8)
        goto read_fail;

      keyframe->numMarkers = num_markers;
      for (num_markers_read = 0; num_markers_read < num_markers; num_markers_read++)
      {
        markers = &keyframe->markers;
        if (!stdConffile_ReadLine())
            break;
        
        if (_sscanf(stdConffile_aLine, "%f %d", &markers->marker_float[num_markers_read], &markers->marker_int[num_markers_read]) != 2)
            break;
      }
      
      if (num_markers_read < num_markers)
            goto read_fail;

      if (!stdConffile_ReadLine())
        goto read_fail;

      if (_sscanf(stdConffile_aLine, " section: %s", std_genBuffer) != 1)
        goto read_fail;
    }
    
    
    if (!stdConffile_ReadLine() || _sscanf(stdConffile_aLine, " nodes %d", &num_nodes) != 1)
    {
      goto read_fail;
    }

    for (nodes_read = 0; nodes_read < num_nodes; nodes_read++)
    {
        if (!stdConffile_ReadLine())
            goto read_fail;
        if (_sscanf(stdConffile_aLine, " node %d", &node_idx) != 1)
            goto read_fail;
        if (!stdConffile_ReadLine())
            goto read_fail;
        if (_sscanf(stdConffile_aLine, " mesh name %s", mesh_name) != 1)
            goto read_fail;
        joint = &keyframe->joints[node_idx];
        
        _strncpy(joint->mesh_name, mesh_name, 0x1Fu);
        joint->mesh_name[31] = 0;
        
        if (!stdConffile_ReadLine())
            goto read_fail;

        if (_sscanf(stdConffile_aLine, " entries %d", &anim_entry_cnt) != 1)
            goto read_fail;

        joint->nodeIdx = node_idx;
        joint->numAnimEntries = anim_entry_cnt;
        joint->animEntries = (rdAnimEntry*)rdroid_pHS->alloc(0x38 * anim_entry_cnt);
        if (!joint->animEntries)
          goto read_fail;

        anim_entry = joint->animEntries;
        for (anim_entry_read = 0; anim_entry_read < joint->numAnimEntries; anim_entry_read++)
        {
            if (!stdConffile_ReadLine()) {
                goto read_fail;
            }
            
            if (_sscanf(
                   stdConffile_aLine,
                   " %d: %f %x %f %f %f %f %f %f",
                   &entry_num,
                   &anim_entry->frameNum,
                   &anim_entry->flags,
                   &pos.x,
                   &pos.y,
                   &pos.z,
                   &orientation.x,
                   &orientation.y,
                   &orientation.z) != 9) {
              goto read_fail;
            }
            
            anim_entry->pos = pos;
            anim_entry->orientation = orientation;
            
            if (!stdConffile_ReadLine()
              || _sscanf(stdConffile_aLine, " %f %f %f %f %f %f", &vel, &vel.y, &vel.z, &angVel, &angVel.y, &angVel.z) != 6)
            {
              goto read_fail;
            }
            anim_entry->vel = vel;
            anim_entry->angVel = angVel;
            anim_entry++;
        }
    }
    
    stdConffile_Close();
    return 1;
  
read_fail:
    stdConffile_Close();
    return 0;
}

int rdKeyframe_Write(char *out_fpath, rdKeyframe *keyframe, char *creation_method)
{
    int fd;
    int totalAnimEntries;
    rdJoint *joint_iter;
    rdAnimEntry *animEntry_iter;
    unsigned int i;
    unsigned int j;

    fd = rdroid_pHS->fileOpen(out_fpath, "wt+");
    if (!fd)
        return 0;

    rdroid_pHS->filePrintf(fd, "# KEYFRAME '%s' created from '%s'\n\n", keyframe, creation_method);
    rdroid_pHS->filePrintf(fd, "###############\n");
    rdroid_pHS->filePrintf(fd, "SECTION: HEADER\n\n");
    rdroid_pHS->filePrintf(fd, "FLAGS  0x%04x\n", keyframe->flags);
    rdroid_pHS->filePrintf(fd, "TYPE   0x%X\n", keyframe->type);
    rdroid_pHS->filePrintf(fd, "FRAMES %d\n", keyframe->numFrames);
    rdroid_pHS->filePrintf(fd, "FPS    %.3f\n", keyframe->fps);
    rdroid_pHS->filePrintf(fd, "JOINTS %d\n", keyframe->numJoints);
    if (keyframe->numMarkers)
    {
        rdroid_pHS->filePrintf(fd, "\n\n");
        rdroid_pHS->filePrintf(fd, "###############\n");
        rdroid_pHS->filePrintf(fd, "SECTION: MARKERS\n\n");
        rdroid_pHS->filePrintf(fd, "MARKERS %d\n\n", keyframe->numMarkers);
        for (i = 0; i < keyframe->numMarkers; i++)
        {
            rdroid_pHS->filePrintf(fd, "%f %d\n", keyframe->markers.marker_float[i], keyframe->markers.marker_int[i]);
        }
    }
    
    rdroid_pHS->filePrintf(fd, "\n\n");
    rdroid_pHS->filePrintf(fd, "###############\n");
    rdroid_pHS->filePrintf(fd, "SECTION: KEYFRAME NODES\n\n");
    totalAnimEntries = 0;
    for (i = 0; i < keyframe->numJoints2; i++)
    {
        if (keyframe->joints[i].numAnimEntries)
            ++totalAnimEntries;
    }
    rdroid_pHS->filePrintf(fd, "NODES %d\n\n", totalAnimEntries);
    joint_iter = keyframe->joints;
    for (i = 0; i < keyframe->numJoints2; i++, joint_iter++)
    {
        if (!joint_iter->numAnimEntries)
            continue;

        rdroid_pHS->filePrintf(fd, "NODE    %d\n", i);
        rdroid_pHS->filePrintf(fd, "MESH NAME %s\n", joint_iter);
        rdroid_pHS->filePrintf(fd, "ENTRIES %d\n", joint_iter->numAnimEntries);
        rdroid_pHS->filePrintf(fd, "\n");
        rdroid_pHS->filePrintf(
        fd,
        "# num:   frame:   flags:           x:           y:           z:           p:           y:           r:\n");
        rdroid_pHS->filePrintf(
        fd,
        "#                                 dx:          dy:          dz:          dp:          dy:          dr:\n");
        animEntry_iter = joint_iter->animEntries;
        for (j = 0; j < joint_iter->numAnimEntries; j++ )
        {
            rdroid_pHS->filePrintf(
                fd,
                " %3d:  %7d   0x%04x %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f\n",
                j,
                animEntry_iter->frameNum,
                animEntry_iter->flags,
                animEntry_iter->pos.x,
                animEntry_iter->pos.y,
                animEntry_iter->pos.z,
                animEntry_iter->orientation.x,
                animEntry_iter->orientation.y,
                animEntry_iter->orientation.z);

            rdroid_pHS->filePrintf(
                fd,
                " %35.8f %12.8f %12.8f %12.8f %12.8f %12.8f\n",
                animEntry_iter->vel.x,
                animEntry_iter->vel.y,
                animEntry_iter->vel.z,
                animEntry_iter->angVel.x,
                animEntry_iter->angVel.y,
                animEntry_iter->angVel.z);
            ++animEntry_iter;
        }
        rdroid_pHS->filePrintf(fd, "\n");
    }
    rdroid_pHS->fileClose(fd);
    
    return 1;
}

void rdKeyframe_FreeEntry(rdKeyframe *keyframe)
{
    if (!keyframe)
        return;

    if (pKeyframeUnloader)
    {
        pKeyframeUnloader(keyframe);
        return;
    }
    
    // This was inlined
    rdKeyframe_FreeJoints(keyframe);
    
    rdroid_pHS->free(keyframe);
}

void rdKeyframe_FreeJoints(rdKeyframe *keyframe)
{
    unsigned int i;
    rdJoint* joint_iter;
    
    if (!keyframe->joints)
        return;

    joint_iter = keyframe->joints;
    for (i = 0; i < keyframe->numJoints2; i++)
    {
        if (joint_iter->animEntries)
        {
            rdroid_pHS->free(joint_iter->animEntries);
            joint_iter->animEntries = NULL;
        }
        joint_iter++;
    }
    rdroid_pHS->free(keyframe->joints);
    keyframe->joints = NULL;
}
