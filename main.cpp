// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unordered_map>
#include <stdio.h>
#include <vector>
#include <deque>
#include <queue>
#include <iostream>
#include <cassert>
#include <string>
#include <array>
#include <algorithm>
#include <stack>
#include <getopt.h>
#include "xcode_redirect.hpp"
#include "logsearch.h"
using namespace std;

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    logbooksearch log_entry_search;
    log_entry_search.get_options(argc, argv);
    log_entry_search.read_file();
    log_entry_search.sort_file();
    log_entry_search.prep_for_candk_search();
    log_entry_search.read_command_and_perform();
    return 0;
}
