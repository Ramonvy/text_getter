#include <iostream>
#include <windows.h>


int ListDirectoryContents(const char *sDir, char list[3000][300], int next_p){
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;

    char sPath[2048];
    int x = next_p;

    //Specify a file mask. *.* = We want everything!
    sprintf(sPath, "%s\\*.*", sDir);

    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE){
        printf("Path not found: [%s]\n", sDir);
        return false;
    }

    do{
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0){
            
			//Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);

            //Is the entity a File or Folder?
            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY){
                strcpy(list[x], fdFile.cFileName);
                ++x;
                ListDirectoryContents(sPath, list, x); //Recursion, I love it!
            }
            else{
            	strcpy(list[x], fdFile.cFileName);
                ++x;
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.

    FindClose(hFind); //Always, Always, clean things up!

    return x;
}


bool endsWith(char *str, char *end){
	int e_len = strlen(end);
	int s_len = strlen(str);
	
	int x = 1;
	
	while((e_len - x) >= 0){
		if(str[s_len - x] != end[e_len - x]){
			return false;
		}
		
		++x;
	}
	
	return true;
}


//Obtem a intercorrencia (interc + 1) de um arquivo cujo nome termina com ext
char *getAFileByExtension(char list[3000][300], int list_sz, char *ext, int interc){
	int x = 0;
	int y = 0;
	
	while(x < list_sz){
		if(endsWith(list[x], ext) == true){
			if(y == interc){
				return list[x];
			}
			else{
				++y;
			}
		}
		++x;
	}
	
	return "fail";
}


//Coverte caracteres erroneamente detectados como 'H' para '#'
void sharp_detect(char *file_name){
	char tmp_file[1000];
	sprintf(tmp_file, "%s.tmp", file_name);
	
	FILE *f1 = fopen(file_name, "rb");
	FILE *f2 = fopen(tmp_file, "wb");
	
	fseek(f1, 0, SEEK_END);
	unsigned long int fsz = ftell(f1);
	fseek(f1, 0, SEEK_SET);
	
	char str[3000];
	int x;
	while(ftell(f1) < fsz){
		fgets(str, 3000, f1);
		
		x = 0;
		while(x < strlen(str) - 1){
			
			if(str[x] == 'H' && str[x + 1] == 'H'){
				str[x] = '#';
				str[x + 1] = '#';
			}
			
			++x;
		}
		
		fprintf(f2, "%s", str);
	}
	
	
	fclose(f1);
	fclose(f2);
	
	char command[1000];
	sprintf(command, "del %s", file_name);
	system(command);
	
	sprintf(command, "rename %s %s", tmp_file, file_name);
	system(command);
}


void pos_proc(char *file_name){
	sharp_detect(file_name);
}


int main(int argc, char** argv) {
	system("del *.txt");
	system("del *.jpg");
	char list[3000][300];
	int list_sz = ListDirectoryContents(".", list, 0);
	char img_base_name[1000];
	strcpy(img_base_name, "image.jpg");
	
	char pdf_name[300];
	strcpy(pdf_name, getAFileByExtension(list, list_sz, ".pdf", 0));
	
	char command[1000];
	sprintf(command, "convert -density 300 %s %s", pdf_name, img_base_name);
	system(command);
	list_sz = ListDirectoryContents(".", list, 0);
	
	char img_name_list[3000][300];
	int img_list_sz = 0;
	char tmp[300];
	
	strcpy(tmp, getAFileByExtension(list, list_sz, ".jpg", img_list_sz));
	
	while(strcmp(tmp, "fail") != 0){
		strcpy(img_name_list[img_list_sz], tmp);
		++img_list_sz;
		strcpy(tmp, getAFileByExtension(list, list_sz, ".jpg", img_list_sz));
	}
	
	if(img_list_sz == 1){
		sprintf(command, "tesseract %s text -l por", img_name_list[0]);
		system(command);
		system("start text.txt");
	}
	else if(img_list_sz > 1){
		int x = 0;
		char name[300];
		strcpy(name, img_base_name);
		name[strlen(name) - 4] = 0;
		
		while(x < img_list_sz){
			sprintf(command, "tesseract %s-%d.jpg %d -l por", name, x, x);
			system(command);
			++x;
		}
		
		system("copy *.txt text.txt");
		system("start text.txt");
	}
	
	return 0;
}



