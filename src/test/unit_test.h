//============================================================================
// Name        : unit_test.h
// Author      : Md Sakib Nizam Khan
// Version     :
// Copyright   : Copyright (C) 2019 SSG-Aalto {msnkhan@kth.se}
// Description :
//============================================================================

#include <stdio.h>
#include "../main/ownership_change.h"
#include "../main/device_info.h"
#include "../main/packet.h"

void test_correctness_of_ip_info_packet_creation(void);
void test_correctness_of_pw_confirmation_packet_creation(void);
void test_correctness_of_challenge_packet_creation(void);
void test_correctness_of_pw_request_packet_creation(void);
void test_correctness_of_profile_list_packet_creation(void);
void test_correctness_of_profile_authetication_response_packet_creation(void);
void test_condition_correctness_of_ready_to_send_challenge(void);
void test_condition_correctness_of_ready_to_send_profile_list(void);
void test_correctness_of_owner_password_retrieval_from_file(void);
