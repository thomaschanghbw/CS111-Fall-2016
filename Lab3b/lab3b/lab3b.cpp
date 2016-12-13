//
//  lab3b.cpp
//  
//
//  Created by Thomas Chang on 11/25/16.
//
//

#include <iostream>
#include<map>
#include <set>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <queue>

using namespace std;


uint32_t hex2int(char *hex) {
    uint32_t val = 0;
    while (*hex) {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

int main()
{
    /** Read all inodes. 
     Create a <block#, <inode#, entry_num> > multimap.
     Used in: Error Check #1
     -----------------------------------------------------
     Create a <block#, inode# > multimap
     Used in: Error Check #2
     -----------------------------------------------------
     Create an inode# set
     Used in: Error Check# 3
     -----------------------------------------------------
     Create a <inode#,link count> multimap
     Used in: Error Check #5
     **/
    multimap<string, pair<string, int> > i_blocknum_inodenum_mmap, i_blocknum_inodenum_only_mmap;
    set<string> i_inode_numbers;
    multimap<string, string> i_inodenum_linkcount_mmap;
    map<string, string> i_inodenum_blocknum_mmap;
    
    string inode_csv_line;
    ifstream inode_file("inode.csv");
    while(getline(inode_file, inode_csv_line))
    {
        string inode_line[26];
        int j = 0;
        // By the end of the for loop, we have a string array that separates the fields of a line of the file
        for(int i = 0; i<26; i++)
        {
            while ( j<inode_csv_line.length())
            {
                if( inode_csv_line[j] == ',')
                {
                    j++;
                    break;
                }
                inode_line[i] += inode_csv_line[j];
                j++;
            }
        }
        /** add to <block#, inode#> multimap. **/
        for (int i = 11; i < 25; i++)
        {
            i_blocknum_inodenum_mmap.insert(pair<string,  pair<string, int> >(inode_line[i], pair<string, int>(inode_line[0], i-11)));
            i_inodenum_blocknum_mmap.insert(pair<string, string>(inode_line[0], inode_line[i]));
            //cout << inode_line[i] << '\n';
        }
        /** add to inode# set **/
         bool isValid = false;
        for(int q = 2; q< 26; q++)
        {
           if(inode_line[q]!="0")
           {
               isValid=true;
               break;
               
           }
        }
        if(isValid)
            i_inode_numbers.insert((inode_line[0]));
        /** add to <inode#,link count> multimap **/
        i_inodenum_linkcount_mmap.insert(pair<string, string>((inode_line[0]), (inode_line[5])));
    }
    inode_file.close();
    
    /** Create a set of free block bitmap blocks (from group descriptors)
     Used in: Error check #1
     -----------------------------------------------------
     Create a set of free inode bitmap blocks (from group descriptors)
     **/
    set<string> gr_block_bitmaps;
    set<string> gr_inode_bitmaps;
    queue <string> gr_i_bitmaps;
    
    string group_csv_line;
    ifstream group_file("group.csv");
    while(getline(group_file, group_csv_line))
    {
        string group_line[7];
        int j = 0;
        for(int i = 0; i<7; i++)
        {
            while ( j<group_csv_line.length())
            {
                if(group_csv_line[j] == ',')
                {
                    j++;
                    break;
                }
               group_line[i] += group_csv_line[j];
                j++;
            }
        }
        gr_block_bitmaps.insert(group_line[5]);
        gr_inode_bitmaps.insert(group_line[4]);
        gr_i_bitmaps.push(group_line[4]);
    }
    group_file.close();
    
    /** Create set of free blocks from bitmap
     Used in: Error Check #1
     Create set of free inodes from inode bitmap 
     Used in: Error Check #4
     **/
    set<string> b_free_blocks;
    set<string> b_free_inodes;
    string bitmap_csv_line;
    ifstream bitmap_file("bitmap.csv");
    while(getline(bitmap_file, bitmap_csv_line))
    {
        string bitmap_line[2];
        int j = 0;
        for(int i = 0; i<2; i++)
        {
            while ( j<bitmap_csv_line.length())
            {
                if(bitmap_csv_line[j] == ',')
                {
                    j++;
                    break;
                }
                bitmap_line[i] += bitmap_csv_line[j];
                j++;
            }
        }
        if (gr_block_bitmaps.find(bitmap_line[0]) != gr_block_bitmaps.end())
        {
            b_free_blocks.insert(bitmap_line[1]);
        }
        if (gr_inode_bitmaps.find(bitmap_line[0]) != gr_inode_bitmaps.end())
        {
            b_free_inodes.insert(bitmap_line[1]);
        }
        
    }
    bitmap_file.close();
    
    set<string>::iterator string_set_iter;
    // note: Add indirect implementation later if needed
    //Error Check #1
    for (string_set_iter=b_free_blocks.begin(); string_set_iter!=b_free_blocks.end();++string_set_iter)
    {
        int block_hex = atoi(string_set_iter->c_str());
        stringstream stream;
        stream << hex << block_hex;
        string block_hex_s (stream.str());
        pair <multimap<string,pair<string, int> >::iterator, multimap<string,pair<string, int> >::iterator> ret;
        ret = i_blocknum_inodenum_mmap.equal_range(block_hex_s);
        if (ret.first!= ret.second)
        {
            cout << "UNALLOCATED BLOCK < " << block_hex << " > REFERENCED BY";
            for (multimap<string,pair<string, int > >::iterator it=ret.first; it!=ret.second; ++it)
            {
                cout << " INODE < " << it->second.first << " > ENTRY < "  << it->second.second << " >";
            }
            cout << '\n';
        }
    }
    
    /** Error Check #2 **/
    //Remove non-duplicates
    i_blocknum_inodenum_only_mmap = i_blocknum_inodenum_mmap;
    for (multimap<string, pair<string, int > >::iterator i = i_blocknum_inodenum_only_mmap.begin(); i!=i_blocknum_inodenum_only_mmap.end();)
    {
        if(i_blocknum_inodenum_only_mmap.count(i->first) == 1)
        {
            i_blocknum_inodenum_only_mmap.erase(i++);
        }
        else
            ++i;
    }
    
    
     //To order the values, convert to a multiset
    multiset<pair<string, pair<string, int> > > ordered_blocknum_inodenum;
    for (multimap<string, pair<string,int> >::iterator i=i_blocknum_inodenum_only_mmap.begin(); i!=i_blocknum_inodenum_only_mmap.end(); i++)
    {
        ordered_blocknum_inodenum.insert(pair<string, pair<string, int> >(i->first, i->second));
    }
    
    //Print duplicate blocks
    multiset<pair<string, pair<string, int> > >::iterator it=ordered_blocknum_inodenum.begin();
    string current_blocknum = it->first;
    int first = 1;
    for (; it!=ordered_blocknum_inodenum.end(); ++it)
    {
        if(it->first!=current_blocknum)
        {
            if(it->first != "0")
            {
                if (first)
                {
            cout << "MULTIPLY REFERENCED BLOCK < " << hex2int(const_cast<char*>(it->first.c_str()))  << " > BY";
            current_blocknum = it->first;
                    first--;
                }
                else
                {

                    cout << "\nMULTIPLY REFERENCED BLOCK < " << hex2int(const_cast<char*>(it->first.c_str())) << " > BY";
                    current_blocknum = it->first;
                }
            }
        }
        if (it->first != "0")
        {
        cout << " INODE < " << it->second.first << " > ENTRY < " << it->second.second << " >";
        }
    }
        cout << '\n';
   
    /** Error Check #4 **/
    // Read the superblock file
    string super_csv_line;
    ifstream super_file("super.csv");
    int number_of_inodes;
    string super_line[9];
    while(getline(super_file, super_csv_line))
    {

        int j = 0;
        // By the end of the for loop, we have a string array that separates the fields of a line of the file
        for(int i = 0; i<9; i++)
        {
            while ( j<super_csv_line.length())
            {
                if( super_csv_line[j] == ',')
                {
                    j++;
                    break;
                }
                super_line[i] += super_csv_line[j];
                j++;
            }
        }
        number_of_inodes = atoi(super_line[1].c_str());
    }
    super_file.close();
    for(int k=11; k!= number_of_inodes; k++)
    {
        stringstream ss;
        ss << k;
        string str = ss.str();

        if ((b_free_inodes.find(str) == b_free_inodes.end()) &&( i_inode_numbers.find(str) == i_inode_numbers.end()))
        {
            int block_num = ( (   atoi(str.c_str()) / atoi(super_line[6].c_str())   ) );
            int p = -1;
            string my_num;
            while (p<block_num)
            {
                my_num = gr_i_bitmaps.front();
                gr_i_bitmaps.pop();
                p++;
            }
            //int block_num =  ( (   atoi(str.c_str()) / atoi(super_line[6].c_str())   ) ) *  (atoi(super_line[5].c_str())) + 4;
            map<string, string>::iterator map_it = i_inodenum_blocknum_mmap.find(str);
            cout << "MISSING INODE < " << str << " > SHOULD BE IN FREE LIST < " << my_num << " >\n";
        }
    }
    
    /** Error Check #5 with some setup for #6 **/
    string dir_csv_line;
    ifstream dir_file("directory.csv");
    multiset<string> dir_inodenum;
    multimap<string, pair<string, int> > dir_inode_parent_entrynum_map;
    multimap<string, string> dir_inode_parent_map;
    multimap<string, string> dir_dot_inode_parent_map;
    multimap<string, string> dir_dotdot_inode_parent_map;
    int entry_number = 0;
    string current_entry = "bleh";
    while(getline(dir_file, dir_csv_line))
    {
        string dir_line[6];
        int j = 0;
        // By the end of the for loop, we have a string array that separates the fields of a line of the file
        for(int i = 0; i<6; i++)
        {
            while ( j<dir_csv_line.length())
            {
                if( dir_csv_line[j] == ',')
                {
                    j++;
                    break;
                }
                dir_line[i] += dir_csv_line[j];
                j++;
            }
        }
        dir_inodenum.insert(dir_line[4]);
        
        
        if (current_entry == "bleh")
        {
            current_entry = dir_line[0];
        }
        else
        {
            if (current_entry != dir_line[0])
            {
                entry_number = 0;
            }
            else {
                entry_number++;
            }
        }
        
        dir_inode_parent_entrynum_map.insert(pair<string, pair<string, int> >(dir_line[4], pair<string, int>(dir_line[0], entry_number)));
        
        if(dir_line[5] == "\".\"")
        {
            dir_dot_inode_parent_map.insert(pair<string,string>(dir_line[4], dir_line[0]));
        }

        else if (dir_line[5] == "\"..\"")
        {
            dir_dotdot_inode_parent_map.insert(pair<string,string>(dir_line[4], dir_line[0]));

        }
        else
        {
            dir_inode_parent_map.insert(pair<string,string>(dir_line[4], dir_line[0]));

        }
    }
    for(int w = 0; w< number_of_inodes; w++)
    {
        stringstream ss;
        ss << w;
        string str = ss.str();
        multimap<string, string>::iterator dir_it = i_inodenum_linkcount_mmap.find(str);
        if(dir_it != i_inodenum_linkcount_mmap.end())
        {
            if(dir_inodenum.count(str) != atoi(dir_it->second.c_str()))
               {
                   cout << "LINKCOUNT < " << str << " > IS < " << dir_it->second << " > SHOULD BE < " << dir_inodenum.count(str) << " >\n";
               }
        }
    }
    
    /** Error Check #3 **/
    multimap<int, pair<string, int> > error_3;
    for ( int c = 11; c< number_of_inodes; c++)
    {
        stringstream ss;
        ss << c;
        string str = ss.str();
        if ((i_inode_numbers.find(str) == i_inode_numbers.end()) && (dir_inodenum.find(str) != dir_inodenum.end()))
        {
            pair<multimap<string, pair<string, int> >::iterator, multimap<string, pair<string, int> >::iterator > ret_it;
            ret_it = dir_inode_parent_entrynum_map.equal_range(str);
            for (multimap<string, pair<string, int> >::iterator it_3 = ret_it.first; it_3 != ret_it.second; it_3++)
            {
                error_3.insert(pair<int, pair<string, int> > (c, pair<string, int>( it_3->second.first, it_3->second.second)));
            }

//            multimap<string, pair<string, int> >::iterator dir_it = dir_inode_parent_entrynum_map.find(str);
//            error_3.insert(pair<int, pair<string, int> >(c, pair<string, int>(dir_it->second.first, dir_it->second.second)));
        }
    }
    for ( int c=11; c<number_of_inodes; c++)
    {
        if ( error_3.count(c) != 0)
        {
            cout << "UNALLOCATED INODE < " << c << " > REFERENCED BY";
            pair<multimap<int, pair<string, int> >::iterator, multimap<int, pair<string, int> >::iterator > ret_it;
            ret_it = error_3.equal_range(c);
            for (multimap<int, pair<string, int> >::iterator it_3 = ret_it.first; it_3 != ret_it.second; it_3++)
            {
                cout << " DIRECTORY < " << it_3->second.first << " > ENTRY < " << it_3->second.second << " >";
            }
            cout <<"\n";
        }
    }
    
    /** Error Check #6 **/
    for(multimap<string,string>::iterator m_it = dir_dot_inode_parent_map.begin(); m_it!= dir_dot_inode_parent_map.end(); m_it++)
    {
        if(m_it->first != m_it->second)
        {
            cout << "INCORRECT ENTRY IN < " << m_it->second << " > NAME < . > LINK TO < " << m_it->first << " > SHOULD BE < " << m_it->second << " >\n";
        }
    }
    for(multimap<string,string>::iterator m_it = dir_dotdot_inode_parent_map.begin(); m_it!=dir_dotdot_inode_parent_map.end(); m_it++)
    {
        multimap<string, string>::iterator m_it2 = dir_inode_parent_map.find(m_it->second);
        if(m_it2 != dir_inode_parent_map.end())
        {
            if(m_it->first != m_it2->second)
            {
                cout << "INCORRECT ENTRY IN < " << m_it->second << " > NAME < .. > LINK TO < " << m_it->first << " > SHOULD BE < " << m_it2->second << " >\n";
            }
        }
    }
    dir_file.close();
    /** Error Check #7 **/
    string in_csv_line;
    ifstream in_file("inode.csv");
    while(getline(in_file, in_csv_line))
    {
        string in_line[26];
        int j = 0;
        // By the end of the for loop, we have a string array that separates the fields of a line of the file
        for(int i = 0; i<26; i++)
        {
            while ( j<in_csv_line.length())
            {
                if( in_csv_line[j] == ',')
                {
                    j++;
                    break;
                }
                in_line[i] += in_csv_line[j];
                j++;
            }
        }
        for (int w= 11; w<11+atoi(in_line[10].c_str()) && w<22; w++)
        {
            if (in_line[w] == "0" || (atoi(in_line[w].c_str()) > atoi(super_line[2].c_str())))
            {
                cout << "INVALID BLOCK < " << in_line[w] << " > IN INODE < " << in_line[0] << " > ENTRY < " << w-11 << " >\n";
            }
            
        }
    }
    

    in_file.close();

//
//    set<string>::iterator iter;
//    for(iter=gr_block_bitmaps.begin(); iter!=gr_block_bitmaps.end();++iter){
//        cout<<(*iter)<<endl;
//    }
    
}