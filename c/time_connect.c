nt32 cur_sec = 0;
ct_uint32 start_sec = 0;

ct_platform_time_sec_get(&start_sec);
printf("---->5, start_sec:%d\n", start_sec);
CT_PORT_RUN(_port_loc_speed_set(port, speed));
ct_platform_time_sec_get(&cur_sec);
printf("---->5.1, cur_sec:%d , speed set cost:%d \n", cur_sec, cur_sec-start_sec);
