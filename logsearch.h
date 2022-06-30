// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
//  logsearch.hpp
//  EECS281Project3
//
//  Created by DIVYANGA on 07/11/21.
//

#ifndef logsearch_h
#define logsearch_h

#include <stdio.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <deque>
#include <queue>
#include <iostream>
#include <cassert>
#include <string>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include <getopt.h>
#include <set>
#include "xcode_redirect.hpp"
using namespace std;


class logbooksearch
{
   private:
   
    struct logentry
    {
        long int timestamp_num = 0;
        string category = ""; //  category in lower case
        string entire_message = "";
        uint32_t entryID = 0;
    };
    
    class logentry_comparator
    {
        public:
        bool operator()(logentry &a, logentry &b) const
        {
            if (a.timestamp_num != b.timestamp_num)
            {
                return a.timestamp_num < b.timestamp_num;
            }
            else if (a.category != b.category)
            {
                return a.category < b.category;
            }
            else
            {
                return a.entryID < b.entryID;
            }
         }
        
    };
    
    class logentry_timestamp_comparator
    {
        public:
        bool operator()(const logentry &a, const logentry &b) const
        {
            return a.timestamp_num < b.timestamp_num;
        }
    };
    
   

    // data structures
    vector<logentry> logbook_masterlist;
    vector<uint32_t> search_result;
    deque<uint32_t> excerpt_list;
    unordered_map<string, vector<uint32_t>> category_search;
    unordered_map<string, vector<uint32_t>> keyword_search;
    vector<uint32_t> original_sorted;
    
    // variables
    string input_file = ""; // reading the file name
    string lower_case_result = ""; // for converting the category into lower case;
    string sentence = ""; // storing the message of each log entry
    string word = "";
    bool search_occured = false;
    long int index_argument = 0;
    uint32_t index_argu = 0;
    uint32_t unsorted_index_first = 0;
    uint32_t unsorted_index_last = 0;
    uint32_t sorted_index_first = 0;
    uint32_t sorted_index_last = 0;
    uint32_t clear_index_first = 0;
    uint32_t clear_index_last = 0;
    vector<logentry>:: iterator it1_t;
    vector<logentry>:: iterator it2_t;
    vector<logentry>:: iterator it1_m;
    vector<logentry>:: iterator it2_m;
    char recent_search = ' ';
    uint32_t lower_bound_position_t = 0;
    uint32_t upper_bound_position_t = 0;
    uint32_t lower_bound_position_m = 0;
    uint32_t upper_bound_position_m = 0;
    string category_final_search = "";
    bool no_keyword = false;
    uint32_t set_size = 0;
    string keyword_1 = "";
    
   public:
    void get_options(int agrc, char **argv);
    void read_file();
    void sort_file();
    void prep_for_candk_search();
    void t_command(string &t_argument);
    void m_command(string &m_argument);
    void c_command(string &category_argument);
    void k_command(string &key_word_sentence);
    void a_command(uint32_t index_a);
    void r_command();
    void d_command(uint32_t index_d);
    void b_command(uint32_t index_b);
    void e_command(uint32_t index_e);
    void s_command();
    void clear_command();
    void print_recent_search_command();
    void print_excerpt_list_command();
    void read_command_and_perform();
};


#endif /* logsearch_h */
