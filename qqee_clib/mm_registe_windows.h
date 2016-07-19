#ifndef MM_REGISTE_WINDOWS_H
#define MM_REGISTE_WINDOWS_H


//windows平台注册表操作库
//编译附加库 -lAdvapi32

#ifdef _cplusplus
extern "C"{
#endif

/**
 * 读注册表项目内容
 * @param  root 目标类型 1:HKEY_CLASSES_ROOT，2:HKEY_CURRENT_CONFIG，3:HKEY_CURRENT_USER，4:HKEY_LOCAL_MACHINE，5:HKEY_USERS
 * @param  path 项目路径: SYSTEM\ControlSet001\Control\Session Manager\Environment\
 * @param  name 项目名称: Path
 * @param  out_len 返回指针字节长度
  * @param  out_type 该条目数据类型
 * @return      成功返回字符串指针(需要手动free)，失败返回负数
char*out;
int out_len=0,out_type=0;
out=mm_registe_get(4,"SYSTEM\\ControlSet001\\Control\\Session Manager\\kernel\\","ObUnsecureGlobalNames",&out_len,&out_type);
printf("%d|%s|%d\n",out_len,out,out_type);
free(out);
 */
char*mm_registe_get(unsigned root,char*path,char*name,unsigned*out_len,unsigned*out_type);


/**
 * 写注册表项
 * @param  root     目标类型
 * @param  path     路径
 * @param  name     项目名称
 * @param  val      写入值
 * @param  cal_type 值类型
 * @param  cal_len  值字节数
 * @return          成功返回1，失败返回负数
 mm_registe_set(4,"SYSTEM\\ControlSet001\\Control\\Session Manager\\kernel\\","testtesttesttest","a\nb\nc\n\n",7,7);
 */
int mm_registe_set(unsigned root,char*path,char*name,char*val,unsigned cal_type,unsigned cal_len);


/**
 * 设置 注册表/文件 权限
 * @param  path          注册表/文件完整路径
 * @param  is_file       是否为文件1=文件，0=注册表
 * @param  power_type    设置模式 1=添加用户、2=设置用户、3=拒绝用户、4=删除用户
 * @param  power         权限 删除权限=65536，完全控制=268435456，运行权限=536870912，读权限=-2147483648，特别的权限=33554432，写权限=1073741824，删除所有用户=32768
 * @param  user_group    用户如User、Administrator、Everyone
 * @param  add_old_power 是否叠加旧权限，1=叠加，其他=新置权限
 * @return               成功返回1，失败返回负数
 mm_registe_set_power("MACHINE\\SYSTEM\\ControlSet001\\Control\\Session Manager\\test",0,2,268435456,"Everyone",0);
 */
int mm_registe_set_power(char*path,int is_file,int power_type,int power,char*user_group,int add_old_power);

#ifdef _cplusplus
}
#endif

#endif