tatic int __dev_info_cmd_get(ct_char *argv1, ct_char *argv2, ct_device_info_t *dev_info)
{
	ct_uint32 num32 = 0;
	ct_uint16 num16 = 0;
	ct_uint16 crc = 0;
	ct_uint32 error = 0;

	if (argv1 == NULL )
	{
		return -1;
	}

	if ((argv2 == NULL && (memcmp(argv1, "crc256", 6) != 0)) || (argv2 == NULL && (memcmp(argv1, "crc4k", 5) != 0)))
	{
		printf("%s\n", argv1);
		printf("crc eroor\n");
		return -1;
	}

	if (memcmp(argv1, "magic", 5) == 0)
	{
		sscanf(argv2,"%x",&num32);
		dev_info->magic_number = num32;
	}
	//else if (memcmp(argv1, "content", strlen(argv1)) == 0)
	else if (strcmp(argv1, "content") == 0)
	{
		sscanf(argv2,"%hx",&num16);
		dev_info->content = num16;
	}
	else if (memcmp(argv1, "length", 6) == 0)
	{
		sscanf(argv2,"%hx",&num16);
		dev_info->length = num16;
	}
	else if (memcmp(argv1, "device", 6) == 0)
	{
		sscanf(argv2,"%hx",&num16);
		dev_info->device_id = num16;
	}
	else if (memcmp(argv1, "hwtype", 6) == 0)
	{
		sscanf(argv2,"%hx",&num16);
		dev_info->hw_type = num16;
	}
	else if (memcmp(argv1, "hwmajor", 7) == 0)
	{
		sscanf(argv2,"%hx",&num16);
		dev_info->hw_major_version = num16;
	}
	else if (memcmp(argv1, "hwminor", 7) == 0)
	{
		sscanf(argv2,"%hx",&num16);
		dev_info->hw_minor_version = num16;
	}
	else if (memcmp(argv1, "mac", 3) == 0)
	{
		memset(dev_info->mac, 0x00, sizeof(dev_info->mac));
		utili_str_to_mac(argv2, dev_info->mac);
	}
	else if (memcmp(argv1, "name", 4) == 0)
	{
		memset(dev_info->product_name, 0x00, sizeof(dev_info->product_name));
		memcpy(dev_info->product_name, argv2, strlen(argv2));
	}
	else if (memcmp(argv1, "binformatver", 12) == 0)
	{
		sscanf(argv2,"%hx",&num16);
		dev_info->bin_format_version = num16;
	}
	else if (memcmp(argv1, "flag", 4) == 0)
	{
		if (memcmp(argv2, "crc", strlen(argv2)) == 0 )
		{
			dev_info->flag = 1;
		}
		else
		{
			dev_info->flag = 0;
		}
	}
	else if (memcmp(argv1, "sn", 2) == 0)
	{
		memset(dev_info->sn, 0x00, sizeof(dev_info->sn));
		memcpy(dev_info->sn, argv2, strlen(argv2));
	}
	else if (memcmp(argv1, "crc256", 6) == 0)
	{
		error = ct_dev_crc256(dev_info, &crc);
		if (error < 0)
		{
			printf("Crc 256 calculate error\n");
		}
		else
		{
			dev_info->crc256 = crc;
			printf("Crc: 0x%04x\n", crc);
		}
	}
	else if (memcmp(argv1, "crc4k", 5) == 0)
	{
		error = ct_dev_crc4k(dev_info, &crc);
		if (error < 0)
		{
			printf("Crc 4k calculate error\n");
		}
		else
		{
			dev_info->crc4k = crc;
			printf("Crc: 0x%04x\n", crc);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

static void __dev_info_show(char *argv, ct_device_info_t dev_info)
{
	ct_uint32 idx = 0;

	if (memcmp(argv, "magic", 5) == 0)
	{	
		printf("0x%08x\n", dev_info.magic_number);
	}
	else if (memcmp(argv, "content", 7) == 0)
	{
		printf("0x%04x\n", dev_info.content);
	}
	else if (memcmp(argv, "length", 6) == 0)
	{
		printf("0x%04x\n", dev_info.length);
	}
	else if (memcmp(argv, "device", 6) == 0)
	{
		printf("0x%04x\n", dev_info.device_id);
	}
	else if (memcmp(argv, "hwtype", 6) == 0)
	{
		printf("0x%04x\n", dev_info.hw_type);
	}
	else if (memcmp(argv, "hwmajor", 7) == 0)
	{
		printf("0x%04x\n", dev_info.hw_major_version);
	}
	else if (memcmp(argv, "hwminor", 7) == 0)
	{
		printf("0x%04x\n", dev_info.hw_minor_version);
	}
	else if (memcmp(argv, "mac", 3) == 0)
	{
		printf("%02x:%02x:%02x:%02x:%02x:%02x\n", dev_info.mac[0], dev_info.mac[1]
				, dev_info.mac[2], dev_info.mac[3], dev_info.mac[4], dev_info.mac[5]);
	}
	else if (memcmp(argv, "name", 4) == 0)
	{
		printf("%s\n", dev_info.product_name);
	}
	else if (memcmp(argv, "crc256", 6) == 0)
	{
		printf("0x%04x\n", dev_info.crc256);
	}
	else if (memcmp(argv, "binformatver", 12) == 0)
	{
		printf("0x%04x\n", dev_info.bin_format_version);
	}
	else if (memcmp(argv, "flag", 4) == 0)
	{
		printf("%s\n", ((dev_info.flag) ? "crc" : "nocrc"));
	}
	else if (memcmp(argv, "sn", 2) == 0)
	{
		for (idx=0;idx<32;idx++)
		{
			printf("%c", dev_info.sn[idx]);
		}
		printf("\n");
	}
	else if (memcmp(argv, "crc4k", 5) == 0)
	{
		printf("0x%04x\n", dev_info.crc4k);
	}
	else if (memcmp(argv, "mtd13", 5) == 0)
	{
		printf("%03x(%2dB) %-25s:0x%08x\n", __device_info.magic_number_ofs, __device_info.magic_number_len, "magic_number", dev_info.magic_number);
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.content_ofs, __device_info.content_len, "content", dev_info.content);
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.length_ofs, __device_info.length_len, "length", dev_info.length);
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.device_id_ofs, __device_info.device_id_len, "device_id", dev_info.device_id);
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.hw_type_ofs, __device_info.hw_type_len, "hw_type", dev_info.hw_type);
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.hw_major_version_ofs, __device_info.hw_major_version_len, "hw_major_version", dev_info.hw_major_version);	
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.hw_minor_version_ofs, __device_info.hw_minor_version_len, "hw_minor_version", dev_info.hw_minor_version);
		printf("%03x(%2dB) %-25s:%02x:%02x:%02x:%02x:%02x:%02x\n", __device_info.mac_ofs, __device_info.mac_len, "mac", dev_info.mac[0], dev_info.mac[1]
				, dev_info.mac[2], dev_info.mac[3], dev_info.mac[4], dev_info.mac[5]);
		printf("%03x(%2dB) %-25s:%s\n", __device_info.product_name_ofs, __device_info.product_name_len, "product_name", dev_info.product_name);
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.crc256_ofs, __device_info.crc256_len, "crc256", dev_info.crc256);

		printf("\n");
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.bin_format_version_ofs, __device_info.bin_format_version_len, "bin_format_version", dev_info.bin_format_version);
		printf("%03x(%2dB) %-25s:%s\n", __device_info.flag_ofs, __device_info.flag_len, "flag", (dev_info.flag) ? "crc" : "nocrc");

		printf("%03x(%2dB) %-25s:", __device_info.sn_ofs, __device_info.sn_len, "sn");
		for (idx=0;idx<32;idx++)
			printf("%c", dev_info.sn[idx]);
		printf("\n");
		printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.crc4k_ofs, __device_info.crc4k_len, "crc4k", dev_info.crc4k);
	}
	else
	{
		printf("Fail parameter error\n");
	}

	if (argc == 2)
	{
		__dev_info_show(argv[1], dev_info);
	}
	else
	{
		if (memcmp(argv[2], "read", 4) == 0)
		{
			if (argv[3] == NULL)
			{
				printf("Fail parameter error\n");
				return -1;
			}
			__dev_info_show(argv[3], dev_info);
		}
		else if (memcmp(argv[2], "write", 5) == 0)
		{
			error = __dev_info_cmd_get(argv[3], argv[4], &dev_info);
			if (error < 0)
			{
				printf("Fail parameter error\n");
				return -1;
			}

			error = ct_dev_info_set(dev_path, &dev_info);
			if (error < 0)
			{
				printf("Fail\n");
				return -1;
			}
			else
			{
				printf("Success\n");
			}
		}
		else
		{
			printf("Fail parameter error\n");
			return -1;
		}
	}*/
