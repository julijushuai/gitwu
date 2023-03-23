
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ct_common_defs.h"
#include "utili_string.h"

#include <mtd/mtd-user.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define OFFSET 0x180000
#define LEN4K 4096
#define LEN256 256
#define MAGIC_NUMBER "0x57575947"
#define SYSFS_MTD "/sys/class/mtd/"
#define DEV_MTD "/dev/"
#define MTD13 "mtd13"

typedef struct {
	ct_uint32 magic_number_ofs;
	ct_uint32 magic_number_len;

	ct_uint32 content_ofs;
	ct_uint32 content_len;

	ct_uint32 length_ofs;
	ct_uint32 length_len;

	ct_uint32 device_id_ofs;
	ct_uint32 device_id_len;

	ct_uint32 hw_type_ofs;
	ct_uint32 hw_type_len;

	ct_uint32 hw_major_version_ofs;
	ct_uint32 hw_major_version_len;

	ct_uint32 hw_minor_version_ofs;
	ct_uint32 hw_minor_version_len;

	ct_uint32 mac_ofs;
	ct_uint32 mac_len;

	ct_uint32 product_name_ofs;
	ct_uint32 product_name_len;

	ct_uint32 crc256_ofs;
	ct_uint32 crc256_len;

	ct_uint32 bin_format_version_ofs;
	ct_uint32 bin_format_version_len;

	ct_uint32 flag_ofs;
	ct_uint32 flag_len;

	ct_uint32 sn_ofs;
	ct_uint32 sn_len;

	ct_uint32 crc4k_ofs;
	ct_uint32 crc4k_len;
} ct_device_info_location_t;

static ct_device_info_location_t __device_info = {
	.magic_number_ofs = 0x0,
	.magic_number_len = 0x4,

	.content_ofs = 0x04,
	.content_len = 0x2,

	.length_ofs = 0x06,
	.length_len = 0x2,

	.device_id_ofs = 0x08,
	.device_id_len = 0x02,

	.hw_type_ofs = 0x0a,
	.hw_type_len = 0x02,

	.hw_major_version_ofs = 0x0c,
	.hw_major_version_len = 0x02,

	.hw_minor_version_ofs = 0x0e,
	.hw_minor_version_len = 0x02,

	.mac_ofs = 0x10,
	.mac_len = 0x06,

	.product_name_ofs = 0x16,
	.product_name_len = 0x10,

	.crc256_ofs = 0x26,
	.crc256_len = 0x02,

	.bin_format_version_ofs = 0x100,
	.bin_format_version_len = 0x02,

	.flag_ofs = 0x102,
	.flag_len = 0x02,

	.sn_ofs = 0x104,
	.sn_len = 0x20,

	.crc4k_ofs = 0xFFE,
	.crc4k_len = 0x02,
};

typedef struct {
	ct_uint32 magic_number;
	ct_uint16 content;
	ct_uint16 length;
	ct_uint16 device_id;
	ct_uint16 hw_type;
	ct_uint16 hw_major_version;
	ct_uint16 hw_minor_version;
	ct_mac mac;
	ct_char product_name[16];
	ct_uint16 crc256;

	ct_uint16 bin_format_version;
	ct_uint16 flag;
	ct_char sn[32];
	ct_uint16 crc4k;
} ct_device_info_t;

typedef enum
{
	CT_DEVICE_INFO_DUMP_E,
	CT_DEVICE_INFO_READ_E,
	CT_DEVICE_INFO_WRITE_E,
} ct_device_info_op_e;

/* Table of CRC constants - implements CRC16-CCITT x16+x12+x5+1 */
static const ct_uint16 crc16_ccitt_tab[] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};


#if 1
//解析获取到的4k数据
static void __parse4k_data(ct_char *mtd_data, ct_device_info_t *dev_info)
{
	ct_char *ofs = NULL;

	ofs = mtd_data;

	dev_info->magic_number = (ofs[0]<<24)|(ofs[1]<<16)|(ofs[2]<<8)|ofs[3];
	ofs += 4;

	dev_info->content = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	dev_info->length = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	dev_info->device_id = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	dev_info->hw_type = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	dev_info->hw_major_version = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	dev_info->hw_minor_version = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	memcpy(dev_info->mac, ofs, 6);
	ofs += 6;

	memcpy(dev_info->product_name, ofs, 16);
	ofs += 16;

	dev_info->crc256 = (ofs[0]<<8)|ofs[1];

	ofs = mtd_data+256;

	dev_info->bin_format_version = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	dev_info->flag = (ofs[0]<<8)|ofs[1];
	ofs += 2;

	memcpy(dev_info->sn, ofs, 32);
	ofs += 32;

	ofs = mtd_data+4094;
	dev_info->crc4k = (ofs[0]<<8)|ofs[1];

}

//fill 256B数据
static void __fill256_data(ct_device_info_t *dev_info, ct_char *space)
{
	ct_char *ofs = NULL;
	
	memset(space, 0x00, LEN256);

	ofs = space;
	ofs[0] = (dev_info->magic_number & 0xFF000000)>>24;
	ofs[1] = (dev_info->magic_number & 0x00FF0000)>>16;
	ofs[2] = (dev_info->magic_number & 0x0000FF00)>>8;
	ofs[3] = (dev_info->magic_number & 0x000000FF);
	ofs += 4;

	ofs[0] = (dev_info->content & 0xFF00)>>8;
	ofs[1] = (dev_info->content & 0x00FF);
	ofs += 2;

	ofs[0] = (dev_info->length & 0xFF00)>>8;
	ofs[1] = (dev_info->length & 0x00FF);
	ofs += 2;

	ofs[0] = (dev_info->device_id & 0xFF00)>>8;
	ofs[1] = (dev_info->device_id & 0x00FF);
	ofs += 2;

	ofs[0] = (dev_info->hw_type & 0xFF00)>>8;
	ofs[1] = (dev_info->hw_type & 0x00FF);
	ofs += 2;

	ofs[0] = (dev_info->hw_major_version & 0xFF00)>>8;
	ofs[1] = (dev_info->hw_major_version & 0x00FF);
	ofs += 2;

	ofs[0] = (dev_info->hw_minor_version & 0xFF00)>>8;
	ofs[1] = (dev_info->hw_minor_version & 0x00FF);
	ofs += 2;

	memcpy(ofs, dev_info->mac, 6);
	ofs += 6;

	memcpy(ofs, dev_info->product_name, 16);
	ofs += 16;

	ofs[0] = (dev_info->crc256 & 0xFF00)>>8;
	ofs[1] = (dev_info->crc256 & 0x00FF);
}

//填充4k数据
static void __fill4k_data(ct_device_info_t *dev_info, ct_char *space)
{
	ct_char *ofs = NULL;
	
	memset(space, 0x00, LEN4K);
	ofs = space;

	__fill256_data(dev_info, space);
	ofs = space+256;

	ofs[0] = (dev_info->bin_format_version & 0xFF00)>>8;
	ofs[1] = (dev_info->bin_format_version & 0x00FF);
	ofs += 2;

	ofs[0] = (dev_info->flag & 0xFF00)>>8;
	ofs[1] = (dev_info->flag & 0x00FF);
	ofs += 2;

	memcpy(ofs, dev_info->sn, 32);

	ofs = space+4094;
	ofs[0] = (dev_info->crc4k & 0xFF00)>>8;
	ofs[1] = (dev_info->crc4k & 0x00FF);
}

#endif
#if 1

static void device_info_help(void)
{
	fprintf(stderr,
	"usage:\n"
		" device-info <device>                                    --printf device info on memory\n"
		" device-info <device> read <option>                      --read the value of option from memory\n"
			"    <option> {magic|content|length|device|hwtype|hwmajor|hwminor|mac|name|crc256|binformatver|flag|sn|crc4k}\n"
		" device-info <device> write <option> <value>             --write to memory\n"
			"    device-info <device> write magic <MagicNumber>       --write <MagicNumber>:0x57575947(WWYL) on memory\n"
			"    device-info <device> write content <Content>         --write 0xAABB 2Bytes on memory\n"
			"    device-info <device> write length <Length>           --write 0xAABB 2Bytes on memory\n"
			"    device-info <device> write device <DeviceId>         --write 0xAABB 2Bytes on memory\n"
			"    device-info <device> write hwtype <HWType>           --write 0xAABB 2Bytes on memory\n"
			"    device-info <device> write hwmajor <MajorVersion>    --write 0xAABB 2Bytes on memory\n"
			"    device-info <device> write hwminor <MinorVersion>    --write 0xAABB 2Bytes on memory\n"
			"    device-info <device> write mac <macaddress>          --write {AABB.CCDD.EEFF|AA:BB:CC:DD:EE:FF} 6Bytes on memory\n"
			"    device-info <device> write name <name>               --write 1~16Bytes on memory\n"
			"    device-info <device> write crc 256                   --calculate first part crc on memory\n"
			"    device-info <device> write binformatver <version>    --a decimal number: current support 1 on memory\n"
			"    device-info <device> write flag {crc|nocrc}          --check crc or no when system startup on memory\n"
			"    device-info <device> write sn <sn>                   --write 1~32Bytes on memory\n"
			"    device-info <device> write crc 4k                    --calculate device info 4k space crc on memory\n"
	);
}

#endif
#if 1
ct_uint16 crc16_ccitt( ct_char *buf, ct_int32 len)
{
	ct_uint32 i = 0;
	ct_uint16 cksum = 0;
	for (i = 0;  i < len;  i++)
	{
		cksum = crc16_ccitt_tab[((cksum>>8) ^ *buf++) & 0xff] ^ (cksum << 8);
	}
	
	return cksum;
}

ct_int32 ct_dev_crc256(ct_device_info_t *dev_info, ct_uint16 *chksum)
{
	ct_char *space256b = NULL;
	ct_uint16 crc = 0;

	space256b = (ct_char *)malloc(LEN256);
	if (space256b == NULL)
	{
		return -1;
	}

	memset(space256b, 0x00, LEN256);
	__fill256_data(dev_info, space256b);
	
	crc = crc16_ccitt(space256b, LEN256);
	*chksum = crc;

	free(space256b);
	return 0;
}

ct_int32 ct_dev_crc4k(ct_device_info_t *dev_info, ct_uint16 *chksum)
{
	ct_char *space4k = NULL;
	ct_uint16 crc = 0;

	space4k = (ct_char *)malloc(LEN4K);
	if (space4k == NULL)
	{
		return -1;
	}

	memset(space4k, 0x00, LEN4K);
	__fill4k_data(dev_info, space4k);
	
	crc = crc16_ccitt(space4k, LEN4K);
	*chksum = crc;

	free(space4k);
	return 0;
}

#endif
#if 1
ct_int32 __dev_info_magic_number_check(ct_uint32 magic_number)
{
	ct_uint32 num = 0;
	
	sscanf(MAGIC_NUMBER,"%x",&num);
	
	if (magic_number != num)
	{
		return -1;
	}

	return 0;
}

#endif
#if 1

ct_int32 ct_dev_info_get(ct_char *path, ct_device_info_t *dev_info)
{
	ct_int32 fd;
	ct_int32 error = 0;
	ct_char *data = NULL;
	
	data = (ct_char *)malloc(LEN4K);
	if (data == NULL)
	{
		return -1;
	}
	//open
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		perror("Open fail ");
		free(data);
		data = NULL;
		return -1;
	}

	//read()
	error = read(fd, data, LEN4K);
	if (error < 0)
	{
		perror("Read fail ");
		close(fd);
		free(data);
		data = NULL;
		return -1;
	}
	
	__parse4k_data(data, dev_info);

	close(fd);
	free(data);
	data = NULL;
	return 0;
}

ct_int32 ct_dev_info_set(ct_char *path, ct_device_info_t *dev_info)
{
	ct_int32 fd;
	ct_int32 error = 0;
	ct_char *data = NULL;
	struct erase_info_user erase;

	//fill data
	data = (ct_char *)malloc(LEN4K);
	if (data == NULL)
	{
		return -1;
	}
	__fill4k_data(dev_info, data);
	
	//open
	fd = open(path, O_RDWR);
	if (fd < 0)
	{
		perror("Open fail ");
		free(data);
		data = NULL;
		return -1;
	}
	
	//erase
	erase.start = 0x00;
	erase.length = LEN4K;
	error = ioctl(fd, MEMERASE, &erase);
	if (error < 0)
	{
		perror("Erase fail ");
		close(fd);
		free(data);
		data = NULL;
		return -1;
	}

	//write
	error = write(fd, data, LEN4K);
	if (error < 0)
	{
		perror("Write fail ");
		close(fd);
		free(data);
		data = NULL;
		return -1;
	}

	close(fd);
	free(data);
	data = NULL;
	return 0;
}

#endif
#if 1

static void __dev_info_magic_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint32 num32 = 0;
	sscanf(argv,"%x",&num32);
	dev_info->magic_number = num32;
}

static void __dev_info_content_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 num16 = 0;
	sscanf(argv,"%hx",&num16);
	dev_info->content = num16;
}

static void __dev_info_length_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 num16 = 0;
	sscanf(argv,"%hx",&num16);
	dev_info->length = num16;
}

static void __dev_info_device_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 num16 = 0;
	sscanf(argv,"%hx",&num16);
	dev_info->device_id = num16;
}

static void __dev_info_hwtype_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 num16 = 0;
	sscanf(argv,"%hx",&num16);
	dev_info->hw_type = num16;
}

static void __dev_info_hwmajor_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 num16 = 0;
	sscanf(argv,"%hx",&num16);
	dev_info->hw_major_version = num16;
}

static void __dev_info_hwminor_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 num16 = 0;
	sscanf(argv,"%hx",&num16);
	dev_info->hw_minor_version = num16;
}

static void __dev_info_mac_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint32 error = 0;
	memset(dev_info->mac, 0x00, sizeof(dev_info->mac));
	error = utili_str_to_mac(argv, dev_info->mac);
	if (error < 0)
	{
		return -1;
	}

}

static void __dev_info_name_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	memset(dev_info->product_name, 0x00, sizeof(dev_info->product_name));
	strncpy(dev_info->product_name, argv, 16);
}

static void __dev_info_binformatver_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 num16 = 0;
	sscanf(argv,"%hx",&num16);
	dev_info->bin_format_version = num16;
}

static void __dev_info_flag_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	if (strcmp(argv, "crc") == 0 )
	{
		dev_info->flag = 1;
	}
	else if (strcmp(argv, "nocrc") == 0)
	{
		dev_info->flag = 0;
	}
	else
	{
		return -1;
	}

}

static void __dev_info_sn_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	memset(dev_info->sn, 0x00, sizeof(dev_info->sn));
	strncpy(dev_info->sn, argv, 32);
}

static void __dev_info_crc_parse(ct_char *argv, ct_device_info_t *dev_info)
{
	ct_uint16 crc = 0;
	if (strcmp(argv, "256") == 0)
	{
		error = ct_dev_crc256(dev_info, &crc);
		if (error < 0)
		{
			printf("Crc256 calculate error\n");
		}
		else
		{
			dev_info->crc256 = crc;
		}
	}
	else if (strcmp(argv, "4k") == 0)
	{
		error = ct_dev_crc4k(dev_info, &crc);
		if (error < 0)
		{
			printf("Crc4k calculate error\n");
		}
		else
		{
			dev_info->crc4k = crc;
		}
	}
	else
	{
		;
	}

}

#endif
#if 1
static void __dev_info_show_all(ct_device_info_t dev_info)
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
	printf("%s\n", dev_info.sn);
	printf("%03x(%2dB) %-25s:0x%04x\n", __device_info.crc4k_ofs, __device_info.crc4k_len, "crc4k", dev_info.crc4k);

}
static void __dev_info_magic_show(ct_device_info_t dev_info)
{
	printf("0x%08x\n", dev_info.magic_number);
}

static void __dev_info_content_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.content);
}

static void __dev_info_length_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.length);
}

static void __dev_info_device_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.device_id);
}

static void __dev_info_hwtype_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.hw_type);
}

static void __dev_info_hwmajor_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.hw_major_version);
}

static void __dev_info_hwminor_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.hw_minor_version);
}

static void __dev_info_mac_show(ct_device_info_t dev_info)
{
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n", dev_info.mac[0], dev_info.mac[1]
			, dev_info.mac[2], dev_info.mac[3], dev_info.mac[4], dev_info.mac[5]);
}

static void __dev_info_name_show(ct_device_info_t dev_info)
{
	printf("%s\n", dev_info.product_name);
}

static void __dev_info_crc256_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.crc256);
}

static void __dev_info_binformatver_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.bin_format_version);
}

static void __dev_info_flag_show(ct_device_info_t dev_info)
{
	printf("%s\n", ((dev_info.flag) ? "crc" : "nocrc"));
}

static void __dev_info_sn_show(ct_device_info_t dev_info)
{
	printf("%s\n", dev_info.sn);
}

static void __dev_info_crc4k_show(ct_device_info_t dev_info)
{
	printf("0x%04x\n", dev_info.crc4k);
}

#endif


typedef void (*__dev_cmd_entry_read_cb)( ct_device_info_t dev_info);
typedef void (*__dev_cmd_entry_prase_cb)(char *argv1, ct_device_info_t dev_info);



typedef struct {
	char *key;
	__dev_cmd_entry_cb read;
	__dev_cmd_entry_prase_cb parse;
} dev_cmd_entry;

static dev_cmd_entry __dev_cmd_entries[] = {
	{"magic", __dev_info_magic_show, __dev_info_magic_parse},
	{"content", __dev_info_content_show, __dev_info_content_parse},
	{"length", __dev_info_length_show, __dev_info_length_parse},
	{"device", __dev_info_device_show, __dev_info_device_parse},
	{"hwtype", __dev_info_hwtype_show, __dev_info_hwtype_parse},
	{"hwmajor", __dev_info_hwmajor_show, __dev_info_hwmajor_parse},
	{"hwminor", __dev_info_hwminor_show, __dev_info_hwminor_parse},
	{"mac", __dev_info_mac_show, __dev_info_mac_parse},
	{"name", __dev_info_name_show, __dev_info_name_parse},
	{"binformatver", __dev_info_binformatver_show, __dev_info_binformatver_parse},
	{"flag", __dev_info_flag_show, __dev_info_flag_parse},
	{"sn", __dev_info_sn_show, __dev_info_sn_parse},
};
int main(int argc, char ** argv)
{
	ct_int32 error = 0;
	ct_uint32 length = 0;
	int idx = 0;
	int find = 0;
	ct_char *dev_name = NULL;
	ct_device_info_t dev_info;
	ct_char dev_path[128];

	dev_name = argv[1];
	length = strlen(DEV_MTD) + strlen(dev_name);
	memset(dev_path, 0, sizeof(dev_path));
	snprintf(dev_path, length + 1, "%s%s", DEV_MTD, dev_name);
	
	memset(&dev_info, 0, sizeof(ct_device_info_t));
	error = ct_dev_info_get(dev_path, &dev_info);
	if (error < 0)
	{
		return -1;
	}
	
	error = __dev_info_magic_number_check(dev_info.magic_number);
	if (error < 0)
	{
		printf("Not device partition\n");
		return -1;
	}
	
	if (argc == 2)
	{
		__dev_info_show_all();
		return 0;
	}

	for (idx = 0; idx < (sizeof(__dev_cmd_entries)/sizeof(dev_cmd_entry)); idx++)
	{
		if (strcmp(argv[3], __dev_cmd_entries[idx].key) == 0)
		{
			find = 1;
			if (strcmp(argv[2], "read") == 0)
			{
				__dev_cmd_entries[idx].read(dev_info);
			}
			else if(strcmp(argv[2], "write") == 0)
			{
				__dev_cmd_entries[idx].parse(argv[4], dev_info);
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
					device_info_help();
				}
		}
	}

	if (find == 0)
	{
		device_info_help();
	}

	return 0;
}

