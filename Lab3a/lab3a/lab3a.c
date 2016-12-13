//
//  lab3a.c
//  
//
//  Created by Thomas Chang on 11/12/16.
//
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//Open, mode_t
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SUPER_BLOCK_OFFSET 1024
#define SUPER_BLOCK_SIZE 1024

/*
 * Structure of the super block
 */
struct super_block {
    uint32_t s_inodes_count; /* Inodes count */
    uint32_t s_blocks_count; /* Blocks count */
    uint32_t s_r_blocks_count; /* Reserved blocks count */
    uint32_t s_free_blocks_count; /* Free blocks count */
    uint32_t s_free_inodes_count; /* Free inodes count */
    uint32_t s_first_data_block; /* First Data Block */
    uint32_t s_log_block_size; /* Block size */
    int32_t s_log_frag_size; /* Fragment size */
    uint32_t s_blocks_per_group; /* # Blocks per group */
    uint32_t s_frags_per_group; /* # Fragments per group */
    uint32_t s_inodes_per_group; /* # Inodes per group */
    uint32_t s_mtime; /* Mount time */
    uint32_t s_wtime; /* Write time */
    uint16_t s_mnt_count; /* Mount count */
    int16_t s_max_mnt_count; /* Maximal mount count */
    uint16_t s_magic; /* Magic signature */
    uint16_t s_state; /* File system state */
    uint16_t s_errors; /* Behaviour when detecting errors */
    uint16_t s_minor_rev_level; /* minor revision level */
    uint32_t s_lastcheck; /* time of last check */
    uint32_t s_checkinterval; /* max. time between checks */
    uint32_t s_creator_os; /* OS */
    uint32_t s_rev_level; /* Revision level */
    uint16_t s_def_resuid; /* Default uid for reserved blocks */
    uint16_t s_def_resgid; /* Default gid for reserved blocks */
    /*
     * These fields are for EXT2_DYNAMIC_REV superblocks only.
     *
     * Note: the difference between the compatible feature set and
     * the incompatible feature set is that if there is a bit set
     * in the incompatible feature set that the kernel doesn't
     * know about, it should refuse to mount the filesystem.
     *
     * e2fsck's requirements are more strict; if it doesn't know
     * about a feature in either the compatible or incompatible
     * feature set, it must abort and not try to meddle with
     * things it doesn't understand...
     */
    uint32_t s_first_ino; /* First non-reserved inode */
    uint16_t s_inode_size; /* size of inode structure */
    uint16_t s_block_group_nr; /* block group # of this superblock */
    uint32_t s_feature_compat; /* compatible feature set */
    uint32_t s_feature_incompat; /* incompatible feature set */
    uint32_t s_feature_ro_compat; /* readonly-compatible feature set */
    uint8_t s_uuid[16]; /* 128-bit uuid for volume */
    char s_volume_name[16]; /* volume name */
    char s_last_mounted[64]; /* directory where last mounted */
    uint32_t s_algorithm_usage_bitmap; /* For compression */
    /*
     * Performance hints.  Directory preallocation should only
     * happen if the EXT2_COMPAT_PREALLOC flag is on.
     */
    uint8_t s_prealloc_blocks; /* Nr of blocks to try to preallocate*/
    uint8_t s_prealloc_dir_blocks; /* Nr to preallocate for dirs */
    uint16_t s_padding1;
    /*
     * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
     */
    uint8_t s_journal_uuid[16]; /* uuid of journal superblock */
    uint32_t s_journal_inum; /* inode number of journal file */
    uint32_t s_journal_dev; /* device number of journal file */
    uint32_t s_last_orphan; /* start of list of inodes to delete */
    uint32_t s_hash_seed[4]; /* HTREE hash seed */
    uint8_t s_def_hash_version; /* Default hash version to use */
    uint8_t s_reserved_char_pad;
    uint16_t s_reserved_word_pad;
    uint32_t s_default_mount_opts;
    uint32_t s_first_meta_bg; /* First metablock block group */
    uint32_t s_reserved[190]; /* Padding to the end of the block */
};
struct group_d
	{
        uint32_t	bg_block_bitmap;		/* Blocks bitmap block */
        uint32_t	bg_inode_bitmap;		/* Inodes bitmap block */
        uint32_t	bg_inode_table;		/* Inodes table block */
        uint16_t	bg_free_blocks_count;	/* Free blocks count */
        uint16_t	bg_free_inodes_count;	/* Free inodes count */
        uint16_t	bg_used_dirs_count;	/* Directories count */
        uint16_t	bg_pad;
        uint32_t	bg_reserved[3];
    };

        struct ext2_inode {
            uint16_t	i_mode;		/* File mode */
            uint16_t	i_uid;		/* Low 16 bits of Owner Uid */
            uint32_t	i_size;		/* Size in bytes */
    		uint32_t	i_atime;	/* Access time */
    		uint32_t	i_ctime;	/* Creation time */
    		uint32_t	i_mtime;	/* Modification time */
    		uint32_t	i_dtime;	/* Deletion Time */
    		uint16_t	i_gid;		/* Low 16 bits of Group Id */
    		uint16_t	i_links_count;	/* Links count */
    		uint32_t	i_blocks;	/* Blocks count */
    		uint32_t	i_flags;	/* File flags */
            union {
                struct {
                    uint32_t  l_i_reserved1;
                } linux1;
                struct {
                    uint32_t  h_i_translator;
                } hurd1;
                struct {
                    uint32_t  m_i_reserved1;
                } masix1;
            } osd1;				/* OS dependent 1 */
            uint32_t	i_block[15];/* Pointers to blocks */
            uint32_t	i_generation;	/* File version (for NFS) */
    		uint32_t	i_file_acl;	/* File ACL */
    		uint32_t	i_dir_acl;	/* Directory ACL */
    		uint32_t	i_faddr;	/* Fragment address */
            union {
                struct {
                    uint8_t	l_i_frag;	/* Fragment number */
                    uint8_t	l_i_fsize;	/* Fragment size */
                    uint16_t	i_pad1;
                    uint16_t	l_i_uid_high;	/* these 2 fields    */
                    uint16_t	l_i_gid_high;	/* were reserved2[0] */
                    uint32_t	l_i_reserved2;
                } linux2;
                struct {
                    uint8_t	h_i_frag;	/* Fragment number */
                    uint8_t	h_i_fsize;	/* Fragment size */
                    uint16_t	h_i_mode_high;
                    uint16_t	h_i_uid_high;
                    uint16_t	h_i_gid_high;
                    uint32_t	h_i_author;
                } hurd2;
                struct {
                        uint8_t	m_i_frag;	/* Fragment number */
                        uint8_t	m_i_fsize;	/* Fragment size */
                        uint16_t	m_pad1;
                        uint32_t	m_i_reserved2[2];
                    } masix2;
            } osd2;				/* OS dependent 2 */
        };

struct dir_entry {
    uint32_t	inode;			/* Inode number */
    uint16_t	rec_len;		/* Directory entry length */
    uint8_t	name_len;		/* Name length */
    uint8_t	file_type;
    char	name[255];	/* File name */
};

int powerOfTwo (unsigned int x)
{
    while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
        x /= 2;
    return (x == 1);
}

void error(char* error_message)
{
    perror(error_message);
    exit(1);
}

void attempt_open(int* fd, char* file_name, mode_t RD)
{
    *fd = open(file_name, RD);
    if (*fd == -1) {
        error("Could not open file at this time");
    }
}

int validateSuperBlock(struct super_block* my_super_block)
{
    if (my_super_block->s_magic != 0xEF53)
    {
        fprintf(stderr, "Superblock - invalid magic: 0x%x\n", my_super_block->s_magic);
        exit(1);
    }
    unsigned int block_size = 1024 << my_super_block->s_log_block_size;
      if(!powerOfTwo((block_size)) || block_size < 512 || block_size > 65000)
      {
          fprintf(stderr, "Superblock - invalid block size: %u\n", (block_size));
          exit(1);
      }
    //** note: missing checks **/
    
    if (my_super_block->s_blocks_count % my_super_block->s_blocks_per_group != 0)
    {
        fprintf(stderr, "Superblock - %i blocks, %i blocks/group\n", my_super_block->s_blocks_count, my_super_block->s_blocks_per_group);
        exit(1);
    }
    if (my_super_block->s_inodes_count % my_super_block->s_inodes_per_group != 0)
    {
        fprintf(stderr, "Superblock - %i blocks, %i blocks/group\n", my_super_block->s_inodes_count, my_super_block->s_inodes_per_group);
        exit(1);
    }
}

void printSuperBlock(struct super_block* my_super_block)
{
    FILE* fd = fopen("super.csv", "a+");
    if(!fd)
    {
        error("Could not open or create super.csv\n");
    }
    fprintf(fd, "%x,%u,%u,%u,%i,%u,%u,%u,%u\n", my_super_block->s_magic, my_super_block->s_inodes_count, my_super_block->s_blocks_count,(1024<<my_super_block->s_log_block_size),(1024<<my_super_block->s_log_frag_size),my_super_block->s_blocks_per_group, my_super_block->s_inodes_per_group,my_super_block->s_frags_per_group, my_super_block->s_first_data_block);
    fclose(fd);
}
void validateGroupDescriptor(int i, unsigned int group_count, unsigned int* blocks_contained, unsigned int blocks_per_group)
{

    if (*blocks_contained != blocks_per_group)
    {
        printf("WE GOTTA PROBLEM. \n");
    }
}
void printGroupDescriptor(struct group_d* my_group_d, unsigned int blocks_contained)
{
    FILE* fd = fopen("group.csv", "a+");
    if (!fd)
    {
        error("Could not open or create group.csv. Please try again. \n");
    }
    fprintf(fd, "%u,%hu,%hu,%hu,%x,%x,%x\n", blocks_contained, my_group_d->bg_free_blocks_count, my_group_d->bg_free_inodes_count, my_group_d->bg_used_dirs_count, my_group_d->bg_inode_bitmap, my_group_d->bg_block_bitmap, my_group_d->bg_inode_table);
    fclose(fd);
}


void printDirectory(struct ext2_inode* my_inode, unsigned int block_size, int fd, unsigned int inode_num)
{
    unsigned char block[block_size];
    struct dir_entry* my_dir_entry;
    unsigned int i;
    unsigned int curr_size = 0;
    unsigned int curr_size_this_entry = 0;
    int done = 0;
    FILE* fd1 = fopen("directory.csv", "a+");
    setvbuf(fd1, NULL, _IONBF, 0);
    unsigned int entry_num = 0;
    for (i=0; i<12; i++)
    {
        pread(fd, block, block_size, SUPER_BLOCK_OFFSET + (my_inode->i_block[i]-1) * block_size);
        my_dir_entry = (struct dir_entry*) block;
        curr_size_this_entry = 0;
        while (curr_size < my_inode->i_size)
        {
            char file_name[256];
            memcpy(file_name, my_dir_entry->name, my_dir_entry->name_len);
            file_name[my_dir_entry->name_len] = 0;
            if(curr_size_this_entry >= block_size)
                break;
            // note: may need to adjust for sparse files
            if(my_dir_entry->inode == 0)
            {
                curr_size += my_dir_entry->rec_len;
                curr_size_this_entry += my_dir_entry->rec_len;
                my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                entry_num++;
                continue;
            }
            fprintf(fd1, "%u,%u,%hu,%u,%u,\"%s\"\n", inode_num, entry_num, my_dir_entry->rec_len, (unsigned int)my_dir_entry->name_len, (unsigned int)my_dir_entry->inode,file_name);
            entry_num++;
            curr_size += my_dir_entry->rec_len;
            curr_size_this_entry += my_dir_entry->rec_len;
            if (curr_size_this_entry < block_size)
            {
                if (curr_size < my_inode->i_size)
                    my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                else {
              
                    done = 1;
                    break;
                }
            }
            else if (curr_size < my_inode->i_size)
            {
                break;
            }
            else {
        
                done = 1;
            }
        }
        if(done)
            break;
    }
    if (done)
        return;
    
    int total_pointers = block_size / sizeof(my_inode->i_block[0]);
    unsigned int indirect_block_offset = SUPER_BLOCK_OFFSET + (my_inode->i_block[12]-1)*block_size;
    int j = 0;
    if (my_inode->i_block[12] != 0)
    {
        for (i=0; i<total_pointers; i++)
        {
            unsigned int address;
            pread(fd, &address, sizeof(address), indirect_block_offset  + (i*sizeof(my_inode->i_block[0])));
            pread(fd, block, block_size, SUPER_BLOCK_OFFSET + (address-1)*block_size);
            my_dir_entry = (struct dir_entry*) block;
            curr_size_this_entry = 0;
            while (curr_size < my_inode->i_size)
            {
                char file_name[256];
                memcpy(file_name, my_dir_entry->name, my_dir_entry->name_len);
                file_name[my_dir_entry->name_len] = 0;
                if(curr_size_this_entry >= block_size)
                    break;
                // note: may need to adjust for sparse files
                if(my_dir_entry->inode == 0)
                {
                    curr_size += my_dir_entry->rec_len;
                    curr_size_this_entry += my_dir_entry->rec_len;
                    my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                    entry_num++;
                    continue;
                }
                fprintf(fd1, "%u,%u,%hu,%u,%u,\"%s\"\n", inode_num, entry_num, my_dir_entry->rec_len, (unsigned int)my_dir_entry->name_len, (unsigned int)my_dir_entry->inode,file_name);
                entry_num++;
                curr_size += my_dir_entry->rec_len;
                curr_size_this_entry += my_dir_entry->rec_len;
                if (curr_size_this_entry < block_size)
                {
                    if (curr_size < my_inode->i_size)
                        my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                    else {
                        
                        done = 1;
                        break;
                    }
                }
                else if (curr_size < my_inode->i_size)
                {
                    break;
                }
                else {
                    
                    done = 1;
                }
            }
        }
    }
     indirect_block_offset = SUPER_BLOCK_OFFSET + (my_inode->i_block[13]-1)*block_size;
    if (my_inode->i_block[13] != 0)
    {
        for (j=0; j<total_pointers; j++)
        {
            unsigned int address;
            pread(fd, &address, sizeof(address), indirect_block_offset  + (j*sizeof(my_inode->i_block[0])));
            for (i=0; i<total_pointers; i++)
            {
                unsigned int address1;
                pread(fd, &address1, sizeof(address1), SUPER_BLOCK_OFFSET + (address-1)*block_size  + (i*sizeof(my_inode->i_block[0])));
                pread(fd, block, block_size, SUPER_BLOCK_OFFSET + (address1-1)*block_size);
                my_dir_entry = (struct dir_entry*) block;
                curr_size_this_entry = 0;
                while (curr_size < my_inode->i_size)
                {
                    char file_name[256];
                    memcpy(file_name, my_dir_entry->name, my_dir_entry->name_len);
                    file_name[my_dir_entry->name_len] = 0;
                    if(curr_size_this_entry >= block_size)
                        break;
                    // note: may need to adjust for sparse files
                    if(my_dir_entry->inode == 0)
                    {
                        curr_size += my_dir_entry->rec_len;
                        curr_size_this_entry += my_dir_entry->rec_len;
                        my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                        entry_num++;
                        continue;
                    }
                    fprintf(fd1, "%u,%u,%hu,%u,%u,\"%s\"\n", inode_num, entry_num, my_dir_entry->rec_len, (unsigned int)my_dir_entry->name_len, (unsigned int)my_dir_entry->inode,file_name);
                    entry_num++;
                    curr_size += my_dir_entry->rec_len;
                    curr_size_this_entry += my_dir_entry->rec_len;
                    if (curr_size_this_entry < block_size)
                    {
                        if (curr_size < my_inode->i_size)
                            my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                        else {
                            
                            done = 1;
                            break;
                        }
                    }
                    else if (curr_size < my_inode->i_size)
                    {
                        break;
                    }
                    else {
                        
                        done = 1;
                    }
                }
            }

        }
    }
    
    indirect_block_offset = SUPER_BLOCK_OFFSET + (my_inode->i_block[14]-1)*block_size;
    int k;
    if (my_inode->i_block[14] != 0)
    {
        for (k=0; k<total_pointers; k++)
        {
            unsigned int address2;
            pread(fd, &address2, sizeof(address2), indirect_block_offset  + (k*sizeof(my_inode->i_block[0])));
            for (j=0; j<total_pointers; j++)
            {
                unsigned int address;
                pread(fd, &address, sizeof(address), SUPER_BLOCK_OFFSET + (address2-1)*block_size + (j*sizeof(my_inode->i_block[0])));
                for (i=0; i<total_pointers; i++)
                {
                    unsigned int address1;
                    pread(fd, &address1, sizeof(address1), SUPER_BLOCK_OFFSET + (address-1)*block_size  + (i*sizeof(my_inode->i_block[0])));
                    pread(fd, block, block_size, SUPER_BLOCK_OFFSET + (address1-1)*block_size);
                    my_dir_entry = (struct dir_entry*) block;
                    curr_size_this_entry = 0;
                    while (curr_size < my_inode->i_size)
                    {
                        char file_name[256];
                        memcpy(file_name, my_dir_entry->name, my_dir_entry->name_len);
                        file_name[my_dir_entry->name_len] = 0;
                        if(curr_size_this_entry >= block_size)
                            break;
                        // note: may need to adjust for sparse files
                        if(my_dir_entry->inode == 0)
                        {
                            curr_size += my_dir_entry->rec_len;
                            curr_size_this_entry += my_dir_entry->rec_len;
                            my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                            entry_num++;
                            continue;
                        }
                        fprintf(fd1, "%u,%u,%hu,%u,%u,\"%s\"\n", inode_num, entry_num, my_dir_entry->rec_len, (unsigned int)my_dir_entry    ->name_len, (unsigned int)my_dir_entry->inode,file_name);
                        entry_num++;
                        curr_size += my_dir_entry->rec_len;
                        curr_size_this_entry += my_dir_entry->rec_len;
                        if (curr_size_this_entry < block_size)
                        {
                            if (curr_size < my_inode->i_size)
                                my_dir_entry = (void*) my_dir_entry + my_dir_entry->rec_len;
                            else {
                            
                                done = 1;
                                break;
                            }
                        }
                        else if (curr_size < my_inode->i_size)
                        {
                            break;
                        }
                        else {
                        
                            done = 1;
                        }
                    }
                }
            
            }
        }
    }
    
    fclose(fd1);
}
void printAndValidateInode(unsigned int inode_table, int group_block_number, unsigned int block_size, int fd, int inode_num_relative, struct super_block* my_super_block, struct group_d* my_group_d)
{
    struct ext2_inode my_inode;
    if (pread(fd, &my_inode, sizeof(my_inode), SUPER_BLOCK_OFFSET + (inode_table-1) * block_size + sizeof(my_inode)*inode_num_relative)==-1)
    {
        error("Problem reading the superblock from the file\n");
    }
    unsigned int inode_num = (group_block_number * my_super_block->s_inodes_per_group) + inode_num_relative +1;
    
    FILE* fd1 = fopen("inode.csv", "a+");
    char file_type = '?';
        if ((my_inode.i_mode & 0xA000) == 0xA000) //Symbolic link
    {
        file_type = 's';
    }
    else  if ((my_inode.i_mode & 0x8000) == 0x8000)
    {
        file_type = 'f';
    }
    else if ((my_inode.i_mode & 0x4000) == 0x4000)
    {
        file_type = 'd';
        printDirectory(&my_inode, block_size, fd, inode_num);
    }
    unsigned int owner_id = ((unsigned int)my_inode.osd2.linux2.l_i_uid_high << 16) + my_inode.i_uid;
    unsigned int group_id = ((unsigned int)my_inode.osd2.linux2.l_i_gid_high << 16) + my_inode.i_gid;
    // note: spec says to detect symbolic links, but the sample output does not?
    // note: this is incorrect - i_size, for regular files, is only the lower 32 bits (this implementation works with the solution, however)
   fprintf(fd1, "%u,%c,%o,%u,%u,%u,%x,%x,%x,%u,%u,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n", inode_num, file_type, my_inode.i_mode,owner_id, group_id, my_inode.i_links_count,my_inode.i_ctime, my_inode.i_mtime, my_inode.i_atime, my_inode.i_size, my_inode.i_blocks/((2<<my_super_block->s_log_block_size)), my_inode.i_block[0], my_inode.i_block[1], my_inode.i_block[2], my_inode.i_block[3], my_inode.i_block[4], my_inode.i_block[5], my_inode.i_block[6], my_inode.i_block[7], my_inode.i_block[8], my_inode.i_block[9], my_inode.i_block[10], my_inode.i_block[11], my_inode.i_block[12], my_inode.i_block[13], my_inode.i_block[14]);
    fclose(fd1);
    
    fd1 = fopen("indirect.csv", "a+");
    unsigned int address;
    unsigned int q;
    int total_pointers = block_size / sizeof(my_inode.i_block[0]);
    if (my_inode.i_block[12]!=0)
    {
        for(q=0; q<total_pointers;q++)
        {
            pread(fd, &address, sizeof(my_inode.i_block[0]), SUPER_BLOCK_OFFSET + (my_inode.i_block[12]-1) * block_size +q*sizeof(my_inode.i_block[0]));
            if (address)
            {
                fprintf(fd1, "%x,%u,%x\n", my_inode.i_block[12], q, address);
            }
        }
        
    }
//    unsigned int r, s;
//    unsigned int address2;
//    if(my_inode.i_block[13]!=0)
//    {
//        for(q=0; q<total_pointers;q++)
//        {
//            pread(fd, &address, sizeof(my_inode.i_block[0]), SUPER_BLOCK_OFFSET + (my_inode.i_block[13]-1) * block_size +q*sizeof(my_inode.i_block[0]));
//            if (address)
//            {
//                fprintf(fd1, "%x,%u,%x\n", my_inode.i_block[13], q, address);
//            }
//        }
//    }
//    if(my_inode.i_block[14]!=0)
//    {
//        for(q=0; q<total_pointers;q++)
//        {
//            pread(fd, &address, sizeof(my_inode.i_block[0]), SUPER_BLOCK_OFFSET + (my_inode.i_block[13]-1) * block_size +q*sizeof(my_inode.i_block[0]));
//            if (address)
//            {
//                fprintf(fd1, "%x,%u,%x\n", my_inode.i_block[13], q, address);
//            }
//        }
//    }
    
    fclose(fd1);
}


void printFreeBlocks_Inodes_Directories(int group_block_number, unsigned int bitmap_offset, unsigned int blocks_per_group, unsigned int inodes_per_group, unsigned int block_size, int fd, struct group_d* my_group_d, struct super_block* my_super_block)
{
    // bytes * (blocks / byte)
    unsigned int block_bitmap_number = bitmap_offset;
   // printf("%i\n", block_bitmap_number);
    
    int inode_bitmap_number = bitmap_offset + 1;
    unsigned char* bitmap = malloc(block_size);
    if (pread(fd, bitmap, block_size, bitmap_offset * block_size)==-1)
    {
        error("Problem reading the superblock from the file\n");
    }
    FILE* fd1 = fopen("bitmap.csv", "a+");
    if (!fd)
    {
        error("Could not open or create bitmap.csv. Please try again. \n");
    }
    int i,j;
    for(i=0; i<block_size; i++)
    {
        for(j=0; j<8; j++)
        {
            if (!((bitmap[i]>>j ) &  0x01))
            {
                fprintf(fd1, "%x,%u\n", block_bitmap_number, 1 + (group_block_number * blocks_per_group) +(8*i) + j);
            }
        }
    }
    
    if (pread(fd, bitmap, block_size, bitmap_offset*block_size + block_size)==-1)
    {
        error("Problem reading the superblock from the file\n");
    }
    for(i=0; i<block_size; i++)
    {
        for(j=0; j<8; j++)
        {
            int table_offset = (8*i)+j;
            if(table_offset < my_super_block->s_inodes_per_group) // note: If this works, add this if statement up higher
            {
                if (!((bitmap[i] >>j)&  0x01))
                {
                    fprintf(fd1, "%x,%u\n", inode_bitmap_number, 1 + (group_block_number * inodes_per_group) +table_offset);
                    //printf("%u\n", group_block_number);
                }
                else
                {
                    printAndValidateInode(my_group_d->bg_inode_table, group_block_number, block_size, fd, table_offset,     my_super_block, my_group_d);
                }
            }
        }
    }
    
    fclose(fd1);
    free(bitmap);

}


int main(int argc, char* argv[])
{
    /** Setup, make sure everything is ready before working on the file system **/
    int fd;
    char* file_name;
    struct super_block my_super_block;
    struct group_d my_group_d;
    
    if (argc ==2 && argv)
    {
        file_name = argv[1];
    }
    else {
        fprintf(stderr, "Incorrect input. Make sure to pass in just one file!\n");
        exit(1);
    }
    /** Read superblock **/
    attempt_open(&fd, file_name, O_RDONLY);
    if (pread(fd, &my_super_block, sizeof(my_super_block), SUPER_BLOCK_OFFSET)==-1)
    {
        error("Problem reading the superblock from the file\n");
    }
    /** Get values from the superblock that we will use later, assuming validateSuperBlock() succeeds **/
    unsigned int group_count = 1 + (my_super_block.s_blocks_count-1) / my_super_block.s_blocks_per_group;
    unsigned int block_size = 1024 << my_super_block.s_log_block_size;
    unsigned int blocks_per_group = my_super_block.s_blocks_per_group;
    unsigned int inodes_per_group = my_super_block.s_inodes_per_group;
    /** Validate superblock note: Do we need to validate every superblock copy?" **/
    validateSuperBlock(&my_super_block);
    printSuperBlock(&my_super_block);
  
    /** Read Group Descriptors **/
    /** note: How do we find the number of blocks without using the superblock? temp solution: subtract address of inode bitmap from block bitmap addresses**/
    int i = 0;
    unsigned int group_block_offset;
    unsigned int group_block_size = sizeof(my_group_d);
    unsigned int blocks_contained = 0;
    for (i=0; i<group_count; i++)
    {
        group_block_offset = SUPER_BLOCK_OFFSET + SUPER_BLOCK_SIZE + (i * group_block_size);
        if(pread(fd, &my_group_d, sizeof(my_group_d),group_block_offset)==-1)
        {
            error("Problem reading the group descriptor from the file\n");
        }
        if (i != group_count - 1)
        {
            blocks_contained = blocks_per_group;
        }
        else
        {
            
        }
        /**Validate Group Descriptor Table and then print the results. Do not print if there is an error **/
        // note: what exactly do we output in case of an error? Nothing, or only certain things??
        //validateGroupDescriptor(i, group_count, &blocks_contained, blocks_per_group);
        printGroupDescriptor(&my_group_d, blocks_contained);
        /** Print free bitmap entries **/
        printFreeBlocks_Inodes_Directories(i, my_group_d.bg_block_bitmap, blocks_per_group, inodes_per_group, block_size, fd, &my_group_d, &my_super_block);
    }
    
  
    /** Read **/
}















