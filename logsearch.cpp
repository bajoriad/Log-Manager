// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
//  logsearch.cpp
//  EECS281Project3
//
//  Created by DIVYANGA on 07/11/21.
//

#include <stdio.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <deque>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <cassert>
#include <string>
#include <array>
#include <algorithm>
#include <stack>
#include <getopt.h>
#include <set>
#include "logsearch.h"
#include "xcode_redirect.hpp"
using namespace std;

void logbooksearch::get_options(int argc, char **argv)
{
    if (argc != 2)
    {
        return;
    }
    
    string command_line = argv[1];
    
    if (command_line == "--help" || command_line == "-h")
    {
        std::cout << "This reads all the log entries of a logbook and produces\n"
                  << "an excerpt list that contain all the important information\n"
                  << "regarding the logbook being looked at\n";
        exit(0);
    }
    else
    {
        input_file = command_line;
    }
}

void logbooksearch::read_file()
{
    ifstream fin(input_file);
    string timestamp_read = "";
    string timestamp_read_copy = "";
    string entire_message_copy = "";
    string category_read = "";
    string category_read_copy = "";
    string message_read = "";
    logentry read_and_push;
    uint32_t index = 0;
    
    if (!fin.is_open())
    {
        cerr << "file could not open";
        return;
    }
    
    while (getline(fin, timestamp_read, '|'))
    {
        timestamp_read_copy = timestamp_read;
        remove(timestamp_read_copy.begin(), timestamp_read_copy.end(), ':');
        read_and_push.timestamp_num = stol(timestamp_read_copy);
        read_and_push.entryID = index;
        ++index;
        getline(fin, category_read, '|');
        category_read_copy = category_read;
        transform(category_read_copy.begin(), category_read_copy.end(), category_read_copy.begin(), ::tolower);
        read_and_push.category = category_read_copy;
        getline(fin, message_read);
        entire_message_copy = timestamp_read + "|" + category_read + "|" + message_read;
        read_and_push.entire_message = entire_message_copy;
        logbook_masterlist.emplace_back(read_and_push);
    }
    cout << index << " entries read" << '\n';
}


void logbooksearch::sort_file()
{
   std::sort(logbook_masterlist.begin(), logbook_masterlist.end(), logentry_comparator());
}

void logbooksearch::prep_for_candk_search()
{
    original_sorted.resize(logbook_masterlist.size());
    for (uint32_t i = 0; i < logbook_masterlist.size(); ++i)
    {
        original_sorted[logbook_masterlist[i].entryID] = i;
        category_search[logbook_masterlist[i].category].push_back(i);
        sentence = logbook_masterlist[i].entire_message.substr(15) + "$";
        for (uint32_t j = 0; j < sentence.size(); ++j)
        {
            if(isalnum(sentence[j]))
            {
                word = word + sentence[j];
            }
            else
            {
                transform(word.begin(), word.end(), word.begin(), ::tolower);
                if (word != "")
                {
                    vector<uint32_t> &v = keyword_search[word];
                    if (v.empty())
                    {
                        v.push_back(i);
                    }
                    else
                    {
                        if (v.back() != i)
                        {
                            v.push_back(i);
                        }
                    }
                }
                word = "";
            }
        }
    }
}

void logbooksearch::t_command(string &t_argument)
{
    string timestamp1_copy = "";
    string timestamp2_copy = "";
    recent_search = 't';
    search_occured = true;
    long int timestamp1 = 0;
    long int timestamp2 = 0;
    logentry lower_testing;
    logentry upper_testing;
    if (t_argument.size() != 29 || t_argument[14] != '|')
    {
        cerr << "Invalid timestamps causing error" << '\n';
        return;
    }
    timestamp1_copy = t_argument.substr(0,14);
    timestamp2_copy = t_argument.substr(15,14);
    remove(timestamp1_copy.begin(), timestamp1_copy.end(), ':');
    remove(timestamp2_copy.begin(), timestamp2_copy.end(), ':');
    timestamp1 = stol(timestamp1_copy);
    timestamp2 = stol(timestamp2_copy);
    lower_testing.timestamp_num = timestamp1;
    upper_testing.timestamp_num = timestamp2;
    it1_t = lower_bound(logbook_masterlist.begin(), logbook_masterlist.end(), lower_testing, logentry_timestamp_comparator());
    it2_t = upper_bound(it1_t, logbook_masterlist.end(), upper_testing, logentry_timestamp_comparator());
    lower_bound_position_t = uint32_t(it1_t - logbook_masterlist.begin());
    upper_bound_position_t = uint32_t(it2_t - logbook_masterlist.begin());
    cout << "Timestamps search: " << (it2_t - it1_t) << " entries found" << '\n';
}

void logbooksearch::m_command(string &m_argument)
{
    recent_search = 'm';
    search_occured = true;
    long int timestamp_m = 0;
    logentry lower_testing_m;
    logentry upper_testing_m;
    if(m_argument.size() != 14)
    {
        cerr << "Invalid timestamp causing error" << '\n';
        return;
    }
    remove(m_argument.begin(), m_argument.end(), ':');
    timestamp_m = stol(m_argument);
    lower_testing_m.timestamp_num = timestamp_m;
    upper_testing_m.timestamp_num = timestamp_m;
    it1_m = lower_bound(logbook_masterlist.begin(), logbook_masterlist.end(), lower_testing_m, logentry_timestamp_comparator());
    it2_m = upper_bound(it1_m, logbook_masterlist.end(), upper_testing_m, logentry_timestamp_comparator());
    lower_bound_position_m = uint32_t(it1_m - logbook_masterlist.begin());
    upper_bound_position_m = uint32_t(it2_m - logbook_masterlist.begin());
    cout << "Timestamp search: " << (it2_m - it1_m) << " entries found" << '\n';
}

void logbooksearch::c_command(string &category_argument)
{
    recent_search = 'c';
    search_occured = true;
    transform(category_argument.begin(), category_argument.end(), category_argument.begin(), ::tolower);
    auto it = category_search.find(category_argument);
    
    if (it == category_search.end())
    {
        category_final_search = "";
        cout << "Category search: 0 entries found" << '\n';
    }
    else
    {
        category_final_search = category_argument;
        cout << "Category search: " << it->second.size() << " entries found" << '\n';
    }
}


void logbooksearch::k_command(string &keyword)
{
    vector<uint32_t> temp_keyword_result;
    no_keyword = false;
    set<string> word_set;
    recent_search = 'k';
    keyword = keyword + "$";
    search_occured = true;
    string keyword_word = "";
    search_result.clear();
    for (uint32_t i = 0; i < keyword.size(); ++i)
    {
        if(isalnum(keyword[i]))
        {
            keyword_word = keyword_word + keyword[i];
        }
        else
        {
            transform(keyword_word.begin(), keyword_word.end(), keyword_word.begin(), ::tolower);
            if (keyword_word != "")
            {
                word_set.insert(keyword_word);
            }
            keyword_word = "";
        }
    }
    set_size = (uint32_t)word_set.size();
    if (set_size == 1)
    {
        auto it_size_1 = keyword_search.find(*(word_set.begin()));
        if (it_size_1 == keyword_search.end())
        {
            keyword_1 = "";
            cout << "Keyword search: 0 entries found" << '\n';
            return;
        }
        else
        {
            keyword_1 = *(word_set.begin());
            cout << "Keyword search: " << it_size_1->second.size() << " entries found" << '\n';
        }
    }
    else if (set_size == 2)
    {
        auto it_size = word_set.begin();
        auto it_size_2 = keyword_search.find(*it_size);
        if (it_size_2 == keyword_search.end())
        {
            no_keyword = true;
            cout << "Keyword search: 0 entries found" << '\n';
            return;
        }
        else
        {
            search_result = it_size_2->second;
            auto it_size_2_copy = keyword_search.find(*(++it_size));
            if (it_size_2_copy == keyword_search.end())
            {
                no_keyword = true;
                cout << "Keyword search: 0 entries found" << '\n';
                return;
            }
            else
            {
                set_intersection(search_result.begin(), search_result.end(), it_size_2_copy->second.begin(), it_size_2_copy->second.end(), back_inserter(temp_keyword_result));
                std::swap(search_result, temp_keyword_result);
                cout << "Keyword search: " << search_result.size() << " entries found" << '\n';
            }
        }
    }
    else
    {
        auto it_1 = word_set.begin();
        auto it_find_1 = keyword_search.find(*it_1);
        if (it_find_1 != keyword_search.end())
        {
            search_result = it_find_1->second;
        }
        
        if (!search_result.empty())
        {
            while((++it_1) != word_set.end())
            {
                auto it_find_2 = keyword_search.find(*it_1);
                if (it_find_2 == keyword_search.end())
                {
                    no_keyword = true;
                    cout << "Keyword search: 0 entries found" << '\n';
                    return;
                }
                else
                {
                    set_intersection(search_result.begin(), search_result.end(), it_find_2->second.begin(), it_find_2->second.end(), back_inserter(temp_keyword_result));
                    std::swap(search_result, temp_keyword_result);
                    temp_keyword_result.clear();
                }
            }
        }
        cout << "Keyword search: " << search_result.size() << " entries found" << '\n';
    }
    
}

void logbooksearch::a_command(uint32_t index_a)
{
    if (index_a >= logbook_masterlist.size())
    {
        cerr << "Invalid integer position. Not in masterlist" << '\n';
        return;
    }
    excerpt_list.push_back(original_sorted[index_a]);
    cout << "log entry " << index_a << " appended" << '\n';
}

void logbooksearch::r_command()
{
    if (search_occured == false)
    {
        cerr << "previous search has not occured" << '\n';
        return;
    }
    if (recent_search == 't')
    {
        for (uint32_t i = lower_bound_position_t; i < upper_bound_position_t; ++i)
        {
            excerpt_list.push_back(i);
        }
        cout << (it2_t - it1_t) << " log entries appended" << '\n';
    }
    else if (recent_search == 'm')
    {
        for (uint32_t i = lower_bound_position_m; i < upper_bound_position_m; ++i)
        {
            excerpt_list.push_back(i);
        }
        cout << (it2_m - it1_m) << " log entries appended" << '\n';
    }
    else if (recent_search == 'c')
    {
        if(category_final_search == "")
        {
            cout << "0 log entries appended" << '\n';
        }
        else
        {
            auto it_c_search = category_search.find(category_final_search);
            excerpt_list.insert(excerpt_list.end(), it_c_search->second.begin(), it_c_search->second.end());
            cout << it_c_search->second.size() << " log entries appended" << '\n';
        }
    }
    else if (recent_search == 'k')
    {
        if(set_size == 1)
        {
            if(keyword_1 == "")
            {
                cout << "0 log entries appended" << '\n';
            }
            else
            {
                auto it_k_search = keyword_search.find(keyword_1);
                excerpt_list.insert(excerpt_list.end(), it_k_search->second.begin(), it_k_search->second.end());
                cout << it_k_search->second.size() << " log entries appended" << '\n';
            }
        }
        else
        {
            if (no_keyword == true)
            {
                cout << "0 log entries appended" << '\n';
            }
            else
            {
                excerpt_list.insert(excerpt_list.end(), search_result.begin(),search_result.end());
                cout << search_result.size() << " log entries appended" << '\n';
            }
        }
    }
}

void logbooksearch::d_command(uint32_t index_d)
{
    if (index_d >= excerpt_list.size())
    {
        cerr << "Invalid integer position. Not in excerpt list" << '\n';
        return;
    }
    excerpt_list.erase(excerpt_list.begin() + index_d);
    cout << "Deleted excerpt list entry " << index_d << '\n';
}

void logbooksearch::b_command(uint32_t index_b)
{
    if (index_b >= excerpt_list.size())
    {
        cerr << "Invalid integer position. Not in excerpt list" << '\n';
        return;
    }
    excerpt_list.push_front(excerpt_list[index_b]);
    excerpt_list.erase(excerpt_list.begin() + index_b + 1);
    cout << "Moved excerpt list entry " << index_b << '\n';
}

void logbooksearch::e_command(uint32_t index_e)
{
    if (index_e >= excerpt_list.size())
    {
        cerr << "Invalid integer position. Not in excerpt list" << '\n';
        return;
    }
    excerpt_list.push_back(excerpt_list[index_e]);
    excerpt_list.erase(excerpt_list.begin() + index_e);
    cout << "Moved excerpt list entry " << index_e << '\n';
}

void logbooksearch::print_excerpt_list_command()
{
    for (uint32_t i = 0; i < excerpt_list.size(); ++i)
    {
        uint32_t index_print = excerpt_list[i];
        cout << i << '|' << logbook_masterlist[index_print].entryID << '|' << logbook_masterlist[index_print].entire_message << '\n';
    }
}

void logbooksearch::print_recent_search_command()
{
    if (search_occured == false)
    {
        cerr << "previous search has not occured" << '\n';
        return;
    }
    if (recent_search == 't')
    {
        for (uint32_t i = lower_bound_position_t; i < upper_bound_position_t; ++i)
        {
            cout << logbook_masterlist[i].entryID << '|' << logbook_masterlist[i].entire_message << '\n';
        }
    }
    else if (recent_search == 'm')
    {
        for (uint32_t i = lower_bound_position_m; i < upper_bound_position_m; ++i)
        {
            cout << logbook_masterlist[i].entryID << '|' << logbook_masterlist[i].entire_message << '\n';
        }

    }
    else if (recent_search == 'c')
    {
        if(category_final_search != "")
        {
            auto it_c_search_p = category_search.find(category_final_search);
            for (uint32_t i = 0; i < it_c_search_p->second.size(); ++i)
            {
                 cout << logbook_masterlist[it_c_search_p->second[i]].entryID << '|' << logbook_masterlist[it_c_search_p->second[i]].entire_message << '\n';
            }
        }
    }
    else if (recent_search == 'k')
    {
        if (set_size == 1)
        {
            if (keyword_1 != "")
            {
                auto it_k_search_p = keyword_search.find(keyword_1);
                for (uint32_t i = 0; i < it_k_search_p->second.size(); ++i)
                {
                     cout << logbook_masterlist[it_k_search_p->second[i]].entryID << '|' << logbook_masterlist[it_k_search_p->second[i]].entire_message << '\n';
                }
            }
        }
        else
        {
            if (no_keyword == false)
            {
                for (uint32_t i = 0; i < search_result.size(); ++i)
                {
                    uint32_t index_print = search_result[i];
                    cout << logbook_masterlist[index_print].entryID << '|' << logbook_masterlist[index_print].entire_message << '\n';
                }
            }
        }
    }
}

void logbooksearch::s_command()
{
    cout << "excerpt list sorted" << '\n';
    if (excerpt_list.empty())
    {
        cout << "(previously empty)" << '\n';
    }
    else
    {
        unsorted_index_first = excerpt_list[0];
        unsorted_index_last = excerpt_list[excerpt_list.size() - 1];
        cout << "previous ordering:" << '\n';
        cout << "0|" << logbook_masterlist[unsorted_index_first].entryID << "|" << logbook_masterlist[unsorted_index_first].entire_message << '\n';
        cout << "..." << '\n';
        cout << (excerpt_list.size() - 1) << "|" << logbook_masterlist[unsorted_index_last].entryID << "|" << logbook_masterlist[unsorted_index_last].entire_message << '\n';
        sort(excerpt_list.begin(), excerpt_list.end(), less<uint32_t>());
        sorted_index_first = excerpt_list[0];
        sorted_index_last = excerpt_list[excerpt_list.size() - 1];
        cout << "new ordering:" << '\n';
        cout << "0|" << logbook_masterlist[sorted_index_first].entryID << "|" << logbook_masterlist[sorted_index_first].entire_message << '\n';
        cout << "..." << '\n';
        cout << (excerpt_list.size() - 1) << "|" << logbook_masterlist[sorted_index_last].entryID << "|" << logbook_masterlist[sorted_index_last].entire_message << '\n';
    }
}

void logbooksearch::clear_command()
{
    cout << "excerpt list cleared" << '\n';
    if (excerpt_list.empty())
    {
        cout << "(previously empty)" << '\n';
    }
    else
    {
        cout << "previous contents:" << '\n';
        clear_index_first = excerpt_list[0];
        clear_index_last = excerpt_list[excerpt_list.size() - 1];
        cout << "0|" << logbook_masterlist[clear_index_first].entryID << "|" << logbook_masterlist[clear_index_first].entire_message << '\n';
        cout << "..." << '\n';
        cout << (excerpt_list.size() - 1) << "|" << logbook_masterlist[clear_index_last].entryID << "|" << logbook_masterlist[clear_index_last].entire_message << '\n';
        excerpt_list.clear();
    }
}

void logbooksearch::read_command_and_perform()
{
    char command = ' ';
    string junk_wrong_cmd = "";
    string comman_related_argu = "";
    do
    {
        cout << "% " ;
        cin >> command;
        if (cin.fail())
        {
               cerr << "cin entered fail state: exiting" << '\n';
               exit(1);
        }
        if (command != 't' && command != 'm' && command != 'c' && command != 'k' && command != 'a' && command != 'r' && command != 'd' && command != 'b' && command != 'e' && command != 's' && command != 'l' && command != 'g' && command != 'p' && command != 'q' && command != '#')
        {
            getline(cin,junk_wrong_cmd);
            cerr << "incorrect command entered" << '\n';
            continue;
        }
       if (command == '#')
        {
            getline(cin,junk_wrong_cmd);
        }
        else if (command == 't')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            t_command(comman_related_argu);
        }
        else if (command == 'm')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            m_command(comman_related_argu);
        }
        else if (command == 'c')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            c_command(comman_related_argu);
        }
        else if (command == 'k')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            k_command(comman_related_argu);
        }
        else if (command == 'a')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            index_argument = stoll(comman_related_argu);
            index_argu = (uint32_t)index_argument;
            a_command(index_argu);
        }
        else if (command == 'r')
        {
            r_command();
        }
        else if (command == 'd')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            index_argument = stoll(comman_related_argu);
            index_argu = (uint32_t)index_argument;
            d_command(index_argu);
        }
        else if (command == 'b')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            index_argument = stoll(comman_related_argu);
            index_argu = (uint32_t)index_argument;
            b_command(index_argu);
        }
        else if (command == 'e')
        {
            cin.ignore();
            getline(cin,comman_related_argu);
            index_argument = stoll(comman_related_argu);
            index_argu = (uint32_t)index_argument;
            e_command(index_argu);
        }
        else if (command == 's')
        {
            s_command();
        }
        else if (command == 'l')
        {
            clear_command();
        }
        else if (command == 'g')
        {
            print_recent_search_command();
        }
        else if (command == 'p')
        {
            print_excerpt_list_command();
        }
     }
     while (command != 'q');
}
