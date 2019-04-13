#pragma once
// This header file is where anything os-specific gets defined
// so that this code can run on windows (and maybe linux) correctly.
#include "stdio.h"
#include "Windows.h"

void print_current_working_directory();
void change_working_directory(char* dir);
void get_run_location(char* loc);