//tool made by B4shCr00k
//For educational purposes only
#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <tlhelp32.h>

#define ok(msg, ...) printf("[+] "msg"\n",##__VA_ARGS__)
#define err(msg, ...) printf("[*] "msg"\n",##__VA_ARGS__)
#define warn(msg, ...) printf("[!] "msg"\n",##__VA_ARGS__)
#define msg(msg, ...) printf("[-] "msg"\n",##__VA_ARGS__)

int ReadMemory(HANDLE hprocess,int value);
LPVOID *foundAddresses = NULL;
int foundCount = 0;


    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
    {
        char windowtitle[300];
        DWORD PID;
        if (IsWindowVisible(hwnd))
        {
            GetWindowTextA(hwnd,windowtitle,sizeof(windowtitle));
            GetWindowThreadProcessId(hwnd,&PID);
            if (windowtitle[0] != '\0')
            {
                msg("Title:%s\nPID:%d",windowtitle,PID);
            }
            
        }
        return TRUE;
        
    }
    void ListVisibleWindows() {
        EnumWindows(EnumWindowsProc, 0); 
    }
    HANDLE GetHandleToProcess(DWORD PID)
    {
       HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID);
       return hprocess;
    }
    int ReadMemory(HANDLE hprocess,int value)
    {
        MEMORY_BASIC_INFORMATION mbi;
        LPVOID start = 0;
        char *contentbuffer;
        LPVOID *tempAddresses = NULL;
        DWORD bytesread;
        int itterations = 0;
        foundCount = 0;
        while (VirtualQueryEx(hprocess,start,&mbi,sizeof(mbi)))
        {
            if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_NOACCESS))
            {
                contentbuffer = (char *)malloc(mbi.RegionSize);
                ReadProcessMemory(hprocess,mbi.BaseAddress,contentbuffer,mbi.RegionSize,&bytesread);
                for (int i = 0; i < bytesread; i++)
                {
                    if (*(int*)&contentbuffer[i] == value)
                    {
                        LPVOID targetaddress = (LPVOID)((DWORD_PTR)mbi.BaseAddress + i);
                        ok("%p\t%d",targetaddress,*(int *)&contentbuffer[i]);
                        tempAddresses = realloc(tempAddresses, (foundCount + 1) * sizeof(LPVOID));
                        tempAddresses[foundCount++] = targetaddress;
                        itterations++;
                    }
                    
                
                }
                free(contentbuffer);
            }
        
            start = (LPVOID)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
            
        }
        foundAddresses = malloc(foundCount * sizeof(LPVOID));
        memcpy(foundAddresses, tempAddresses, foundCount * sizeof(LPVOID));
        free(tempAddresses);
        
        msg("Read %d",itterations);
        return itterations;
    }
    void WriteToMemory(HANDLE hprocess,LPCVOID newval,LPVOID address)
    {
        DWORD byteswritten;
        WriteProcessMemory(hprocess,address,newval,sizeof(int),&byteswritten);
    }



    void ReScan(HANDLE hprocess,int NewValue)
    {   
    
        DWORD bytesread;
        int itterations = 0;
        LPVOID *tempAddresses = NULL;
        int newCount = 0;
        
        for (int i = 0; i < foundCount; i++)
        {
            LPVOID addr = foundAddresses[i];
            int *contentbuffer = (int *)malloc(sizeof(int)); 
            
            ReadProcessMemory(hprocess,addr,contentbuffer,sizeof(int),&bytesread);
            if (*contentbuffer == NewValue)
            {
                ok("%p\t%d",addr,*contentbuffer);
                tempAddresses = realloc(tempAddresses, (newCount + 1) * sizeof(LPVOID));
                tempAddresses[newCount++] = addr;
                itterations++;
            }
            free(contentbuffer);
        }
        msg("Read %d",itterations);
        
        free(foundAddresses); 
        foundAddresses = tempAddresses;
        foundCount = newCount; 
        

    }

    int ListAllProcesses()
    {
        PROCESSENTRY32 snapshots;
        snapshots.dwSize = sizeof (PROCESSENTRY32);
        HANDLE hsnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
        Process32First(hsnapshot,&snapshots);
        ok("%s\nPID:%d",snapshots.szExeFile,snapshots.th32ProcessID);
        while (Process32Next(hsnapshot, &snapshots))
        {
            msg("%s\nPID:%d",snapshots.szExeFile,snapshots.th32ProcessID);
        }
        
        
        return 0;
        
    }


    int main()
    {
        DWORD PID;
        int value;
        int choice;
        char choice2 [10];
        int newval;
        LPVOID address;
        msg("1 > Scan For Open Windows Only ");
        msg("2 > Scan For All Processes");
        printf("> ");
        scanf("%d",&choice);
        if (choice == 1)
        {
            ListVisibleWindows();
        }
        else if (choice == 2)
        {
            ListAllProcesses();
        }
        else
        {
            err("Invalid Choice !");
            return 1;
        }
        printf("[-] Enter PID :");
        scanf("%d",&PID);

        HANDLE hprocess = GetHandleToProcess(PID);
        printf("[-] Enter Value :");
        scanf("%d",&value);
        ReadMemory(hprocess,value);
        while (1)
        {
            
            printf("[-] Enter Value :");
            scanf("%d",&value);
            ReScan(hprocess,value);
            msg("MeM0Ry H4Ck?");
            warn("e To Edit Memory Address");
            warn("s To Scan Again");
            warn("q To Leave");
            printf("> ");
            scanf("%s",choice2);
            if (strcmp(choice2,"e") == 0)
            {
                printf("[-] Enter New Address :");
                scanf("%p",&address);
                printf("[-] Enter New Value :");
                scanf("%d",&newval);
                WriteToMemory(hprocess,&newval,address);
                ok("Mem0ry H4cKeD Succefuly ;)");
            }
            else if (strcmp(choice2,"s") == 0)
            {
                
                continue;
            }
            else if (strcmp(choice2,"q") == 0)
            {
                msg("Thank Y0u F0r Us1ng My T00l <333");
                break;
            }
            
            else
            {
                err("Invalid Choice");
                break;
            }   
        }
        
        return 0;  
    } 
