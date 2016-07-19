//windows平台注册表操作库
//编译附加库 -lAdvapi32


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <winreg.h>
//#include <accctrl.h>
#include <aclapi.h>
#include "mm_registe_windows.h"



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
char*mm_registe_get(unsigned root,char*path,char*name,unsigned*out_len,unsigned*out_type){
	int32_t hwd=0;
	int32_t hwd2=0;
	int32_t intA=0;
	int32_t n=0;
	int32_t i=0;
	char*ret;
	char*ret2;
	char*ret2P;
	switch(root){
		case 1:
			hwd=2147483648;
			break;
		case 2:
			hwd=2147483648+5;
			break;
		case 3:
			hwd=2147483648+1;
			break;
		case 4:
			hwd=2147483648+2;
			break;
		case 5:
			hwd=2147483648+3;
			break;
		default:
			return (char*)-1;
	}
	RegOpenKeyExA((HKEY)hwd, (LPCTSTR)path, 0, 1, (PHKEY)&hwd2);
	if(hwd2!=0){
		RegQueryValueExA((HKEY)hwd2, name, 0, &intA, NULL, &n);
		ret=malloc(n+1);
		memset(ret,0,n+1);
		RegQueryValueExA((HKEY)hwd2, name, 0, &intA, ret, &n);
		*out_len=n;
		*out_type=intA;
		if(intA==7){
			//printf("NeedReplace\n");
			ret2=malloc(n*2+1);
			ret2P=ret2;
			memset(ret2,0,n*2+1);
			for(i=0;i<n;i++){
				if(*ret=='\0'){
					//memcpy(ret2P,"\n",1);
					*(ret2P++) = '\n';
					++ret;
				}else{
					*(ret2P++) = *(ret++);
				}
			}
			free(ret);
			return ret2;
		}
		RegCloseKey((HKEY)hwd2);
		return ret;
	}
	return (char*)-2;
}


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
int mm_registe_set(unsigned root,char*path,char*name,char*val,unsigned cal_type,unsigned cal_len){
	int32_t hwd=0;
	int32_t hwd2=0;
	int32_t n,i;
	char*valP=val;
	char*valP2=NULL;
	char*vvv=val;
	switch(root){
		case 1:
			hwd=2147483648;
			break;
		case 2:
			hwd=2147483648+5;
			break;
		case 3:
			hwd=2147483648+1;
			break;
		case 4:
			hwd=2147483648+2;
			break;
		case 5:
			hwd=2147483648+3;
			break;
		default:
			return -1;
	}
	if(cal_type==7){
		valP2=malloc(cal_len*2+1);
		vvv=valP2;
		memset(valP2,0,cal_len*2+1);
		for(i=0;i<n;i++){
			if(*valP=='\n'){
				*(valP2++) = '\0';
				++valP;
			}else{
				*(valP2++) = *(valP++);
			}
		}
	}
	RegOpenKeyExA((HKEY)hwd, (LPCTSTR)path, 0, 2, (PHKEY)&hwd2);
	if(hwd2!=0){
		i=RegSetValueExA((HKEY)hwd2, name, 0, cal_type, vvv, cal_len+1);
		RegCloseKey((HKEY)hwd2);
	}
	if(valP2!=NULL)free(valP2);
	return (i==0)?1:-2;
}


/*
typedef struct tag_BuildExplicitAccessWithName_DB{
	int aa;
	int bb;
	int cc;
	int dd;
	int ee;
}BuildExplicitAccessWithName_DB;
typedef struct tag_BuildExplicitAccessWithNameA_DATA{
	int a;
	int b;
	int c;
	BuildExplicitAccessWithName_DB*d;
}BuildExplicitAccessWithNameA_DATA;
*/
//WINADVAPI DWORD WINAPI GetNamedSecurityInfoA(LPSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID*,PSID*,PACL*,PACL*,PSECURITY_DESCRIPTOR*);
//WINADVAPI DWORD WINAPI SetNamedSecurityInfoA(LPSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL);
//WINADVAPI VOID WINAPI BuildExplicitAccessWithNameA(int,char*,int,int,int);
//WINADVAPI VOID WINAPI BuildExplicitAccessWithNameA(PEXPLICIT_ACCESS_A,LPSTR,DWORD,ACCESS_MODE,DWORD);
//WINADVAPI DWORD WINAPI SetEntriesInAclA(ULONG,PEXPLICIT_ACCESS_A,PACL,PACL*);
//WINADVAPI DWORD WINAPI SetNamedSecurityInfoA(LPSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL);
//WINBASEAPI HLOCAL WINAPI LocalFree(HLOCAL hMem);
/**
 * 设置 注册表/文件 权限
 * @param  path          注册表/文件完整路径
 * @param  is_file       是否为文件1=文件，0=注册表
 * @param  power_type    设置模式 1=添加用户、2=设置用户、3=拒绝用户、4=删除用户
 * @param  power         权限 删除权限=65536，完全控制=268435456，运行权限=536870912，读权限=-2147483648，特别的权限=33554432，写权限=1073741824，删除所有用户=32768
 * @param  user_group    用户如User、Administrator、Everyone
 * @param  add_old_power 是否叠加旧权限，1=叠加，其他=新置权限
 * @return               成功返回1，失败返回负数
mm_registe_set_power("MACHINE\\SYSTEM\\ControlSet001\\Control\\Session Manager\\test",0,2,32768,"Everyone",0);
 */
int mm_registe_set_power(char*path,int is_file,int power_type,int power,char*user_group,int add_old_power){
	//BuildExplicitAccessWithNameA_DATA BuildExplicitAccessWithNameA_DATA_tmp;
	PEXPLICIT_ACCESS_A BuildExplicitAccessWithNameA_DATA_tmp;
	char*pathsave=path;
	int h1,h2,h3,h4;
	h4=h3=h2=h1=0;
	if(add_old_power==1){
		if(is_file){
			h1=GetNamedSecurityInfoA((LPSTR)pathsave,1,4, 0, 0, (PACL*)&h2, 0, (PSECURITY_DESCRIPTOR*)&h3);

		}else{
			h1=GetNamedSecurityInfoA((LPSTR)pathsave,4,4, 0, 0, (PACL*)&h2, 0, (PSECURITY_DESCRIPTOR*)&h3);
		}
	}else{
		if(is_file){
			h1=SetNamedSecurityInfoA((LPSTR)pathsave,1,4,0,0,0,0);
		}else{
			h1=SetNamedSecurityInfoA((LPSTR)pathsave,4,4,0,0,0,0);
		}
	}
	//printf("1=%d\n",h1);
	//BuildExplicitAccessWithNameA((PEXPLICIT_ACCESS_A)&BuildExplicitAccessWithNameA_DATA_tmp,user_group, power, power_type,3);
	//BuildExplicitAccessWithNameA(&BuildExplicitAccessWithNameA_DATA_tmp,user_group, power, power_type,3);
	
	//WINADVAPI VOID WINAPI BuildExplicitAccessWithNameA(PEXPLICIT_ACCESS_A,LPSTR,DWORD,ACCESS_MODE,DWORD);
	BuildExplicitAccessWithNameA(BuildExplicitAccessWithNameA_DATA_tmp,user_group,power,power_type,3);
	
	//printf("1=%d\n",h1);
	h1=SetEntriesInAclA(1, (PEXPLICIT_ACCESS_A)&BuildExplicitAccessWithNameA_DATA_tmp, (PACL)h2, (PACL*)&h4);
	//printf("2=%d\n",h1);
	if(is_file==1){
		h1=SetNamedSecurityInfoA((LPSTR)pathsave, 1, 4, 0, 0, (PACL)h4, 0);
	}else{
		h1=SetNamedSecurityInfoA((LPSTR)pathsave, 4, 4, 0, 0, (PACL)h4, 0);
	}
	//printf("3=%d\n",h1);
	if(h4>0)LocalFree((HLOCAL)h4);
	if(h3>0)LocalFree((HLOCAL)h3);
	return 1;
}


/*int main(int argc, char const *argv[]){
	char*out;
	int out_len=0,out_type=0;
	out=mm_registe_get(4,"SYSTEM\\ControlSet001\\Control\\Session Manager\\kernel\\","ObUnsecureGlobalNames",&out_len,&out_type);
	printf("%d|%s|%d\n",out_len,out,out_type);
	free(out);

	//mm_registe_set(4,"SYSTEM\\ControlSet001\\Control\\Session Manager\\kernel\\","testtesttesttest","a\nb\nc\n\n",7,7);

	mm_registe_set_power("MACHINE\\SYSTEM\\ControlSet001\\Control\\Session Manager\\test",0,2,32768,"Everyone",0);

	return 0;
}*/