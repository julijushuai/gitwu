
ob_buf oob;
erase_info_user erase;
mtd_info_user meminfo;

/* 获得设备信息 */
if(0 != ioctl(fd, MEMGETINFO, &meminfo))
	perror("MEMGETINFO");

	/* 擦除块 */
if(0 != ioctl(fd, MEMERASE, &erase))
	perror("MEMERASE");

	/* 读OOB */
if(0 != ioctl(fd, MEMREADOOB, &oob))
	perror("MEMREADOOB");

	/* 写OOB??? */
if(0 != ioctl(fd, MEMWRITEOOB, &oob))
	perror("MEMWRITEOOB");

	/* 检查坏块 */
	if(blockstart != (ofs & (~meminfo.erase + 1))){
		blockstart = ofs & (~meminfo.erasesize + 1);
		if((badblock = ioctl(fd, MEMGETBADBLOCK, &blockstart)) < 0)
			perror("MEMGETBADBLOCK");
		else if(badblock)
			/* 坏块代码 */
		else
			/* 好块代码 */
	}
