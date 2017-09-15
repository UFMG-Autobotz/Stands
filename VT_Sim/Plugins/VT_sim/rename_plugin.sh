#! /bin/bash

Name="my_Model"
TemplateName="ModelName"
FILE_LIST="CMakeLists.txt compile.sh run.sh ModelName.world ModelName_plugin.cpp"

if [ "$#" -eq 1 ]
then
	Name="$1"
else
	if [ "$#" -eq 2 ]
	then
		Name="$1"
		TemplateName="$2"
	else
		if [ "$#" -ge 3 ]
			then
				Name="$1"
				TemplateName="$2"
				shift 2
				FILE_LIST="$@"
		fi
	fi
fi

upperName=$(perl -e "print uc('$Name');")
lowerName=$(perl -e "print lc('$Name');")

TemplateupperName=$(perl -e "print uc('$TemplateName');")
TemplatelowerName=$(perl -e "print lc('$TemplateName');")

perl -pi -w -e "s/$TemplateName/$Name/g;" $FILE_LIST
perl -pi -w -e "s/$TemplateupperName/$upperName/g;" $FILE_LIST
perl -pi -w -e "s/$TemplatelowerName/$lowerName/g;" $FILE_LIST

rename  "s/$TemplateName/$Name/" $FILE_LIST
rename  "s/$TemplateupperName/$upperName/" $FILE_LIST
rename  "s/$TemplatelowerName/$lowerName/" $FILE_LIST