#!/bin/bash
release=`cat /etc/*-release`

checkOS(){
	osString=''
	
	flag=`isCentOS`
	if [[ $flag == True ]];then
		osString=${osString}'centos'
		substr=$(echo $release | grep "release 7")
		if [[ $substr != '' ]];then
			osString=${osString}"7"
		else
			substr=$(echo $release | grep "release 6")
			if [[ $substr != '' ]];then
				osString=${osString}"6"
			else
				substr=$(echo $release | grep "release 5")
				if [[ $substr != '' ]];then
					osString=${osString}"5"
				else
					substr=$(echo $release | grep "release 4")
					if [[ $substr != '' ]];then
						osString=${osString}"4"
					else
						osString=''
					fi
				fi
			fi
		fi
	else
		flag=`isRedHat`
		if [[ $flag == True ]];then
			osString=${osString}"el"
			substr=$(echo $release | grep "release 7")
			if [[ $substr != '' ]];then
				osString=${osString}"7"
			else
				substr=$(echo $release | grep "release 6")
				if [[ $substr != '' ]];then
					osString=${osString}"6"
				else
					substr=$(echo $release | grep "release 5")
					if [[ $substr != '' ]];then
						osString=${osString}"5"
					else
						substr=$(echo $release | grep "release 4")
						if [[ $substr != '' ]];then
							osString=${osString}"4"
						else
							osString=''
						fi
					fi
				fi
			fi
		else
			flag=`isSUSE`
			if [[ $flag == True ]];then
				osString=${osString}"sles"
				substr=$(echo $release | grep "VERSION = 10")
				if [[ $substr != '' ]];then
					osString=${osString}"10"
				else
					substr=$(echo $release | grep "VERSION = 11")
					if [[ $substr != '' ]];then
						osString=${osString}"11"
					else
						osString=''
					fi
				fi
			else
				flag=`isUbuntu`
				if [[ $flag == True ]];then
					osString=${osString}"ubuntu"
					substr=$(echo $release | grep "VERSION = 10")
				fi
			fi
		fi
		
	fi



	if [[ $osString == "" ]];then
		echo "Unknow"
		return
	fi
	echo $osString
}

isCentOS(){
	substr=$(echo $release | grep "CentOS")
	if [[ $substr != '' ]];then
		echo True
	else
		echo False
	fi
}

isRedHat(){
	substr=$(echo $release | grep "Red Hat")
	if [[ $substr != '' ]];then
		echo True
	else
		echo False
	fi
}

isSUSE(){
	substr=$(echo $release | grep "SUSE")
	if [[ $substr != '' ]];then
		echo True
	else
		echo False
	fi
}

isUbuntu(){
	substr=$(echo $release | grep "Ubuntu")
	if [[ $substr != '' ]];then
		echo True
	else
		echo False
	fi
}

os=`checkOS`
echo $os
