#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <stdio.h>


#define MASK_FOUND L"*"
#define Size 256
#define BufferSize 256
#define BUFSIZE 256

using namespace std;

HANDLE CHILD_IN_R = 0;
HANDLE CHILD_IN_W = 0;
HANDLE CHILD_OUT_R = 0;
HANDLE CHILD_OUT_W = 0;
HANDLE inputfile = NULL;


void CreateDirSystem(LPSTR Dir,int i)//create System of Directories (recursive)
{
    if(i<14)
    {
        char buf[256];
        sprintf(buf,"%s\\FILE%d",Dir,i);
        i++;
        CreateDirectory(buf,NULL);
        CreateDirSystem(buf,i);
    }
}


void CopyDirSystem(LPSTR Dir, LPSTR Dir2)
{
    char buf[256]={0};
    char buf2[256]={0};
    sprintf(buf, "%s\\%s", Dir, "*");

    WIN32_FIND_DATA FindFileData;
    HANDLE hf;
    hf=FindFirstFile(buf, &FindFileData);

    int count = 0;
    if (hf!=INVALID_HANDLE_VALUE)
    {
        do
        {
            sprintf(buf, "%s\\%s", Dir, FindFileData.cFileName);

            cout<<buf<<"\n";

            if( !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
                cout<<"File: "<<FindFileData.cFileName<<endl;
                count++;
                sprintf(buf2, "%s\\%s", Dir2, FindFileData.cFileName);
                CreateFile(buf2,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
                CopyFile(buf,buf2,FALSE);
                CopyDirSystem(buf,Dir2);
            }
        }
        while (FindNextFile(hf,&FindFileData)!=0);
        cout<<count<<endl;
        FindClose(hf);
    }
}

//ÁÅÑÊÎÍÅ×ÍÎ ÏÀÏÎÊ:
/*void CreateDirSystem(LPSTR Dir)
{
    char buf[256];
    int i = 11;
    sprintf(buf,"%s\\FILE%d",Dir,i++);
    do
    {
        CreateDirectory(buf,NULL);
        CreateDirSystem(buf);
    }while(i!=15);
}
*/
void ChildProcess(LPCSTR process1,LPCSTR newvalue,STARTUPINFO si1,PROCESS_INFORMATION pi1)
{
    //STARTUPINFO si1 = {0};
    //PROCESS_INFORMATION pi1 = {0};

    //Cout << old PATH:
    cout<<"Your Old PATH Environment: "<<getenv("path")<<endl;

    //Copy environment value(path):
    char buffer [BufferSize];//buffer for GetEnvironmentValue;
    DWORD old = GetEnvironmentVariable(TEXT("path"),buffer,BufferSize);//Copy to buffer, function return COUNT of symbols that was copied;
    cout<<endl<<"Your symbols count in buffer: "<<old<<endl;//If function is OK, it will return count;

    //Let`s change environment to child process:
    newvalue = "C:\\";
    BOOL sev = SetEnvironmentVariable("path",newvalue);
    if(sev != TRUE)
        printf("Error With Your SetEnvironmentVariable Function!\n");

    //Create Child Process, that will have new Environment(with our changes : newvalue);
    if(!CreateProcess(NULL, LPSTR(process1),NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&si1,&pi1) )
    {
        cout<<"Your CreateProcess (cr_p) ERROR!"<<GetLastError()<<endl;
    }
    //Return old PATH value:
    SetEnvironmentVariable("path",buffer);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);

}


void WriteToPipe()
{
    DWORD dwRead, dwWritten;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;

    for(;;)
    {
       bSuccess = ReadFile(inputfile, chBuf, BUFSIZE, &dwRead, NULL);

       bSuccess = WriteFile(CHILD_IN_R, chBuf, dwRead, &dwWritten, NULL);
    }
}

void ReadFromPipe()
{
    DWORD dwRead, dwWritten;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    for (;;)
    {
       bSuccess = ReadFile( CHILD_OUT_R, chBuf, BUFSIZE, &dwRead, NULL);
       //`cout<<endl<<endl<<endl;
       if( ! bSuccess || dwRead == 0 )
       {
           cout<<"Break1";
           //break;
       }

       bSuccess = WriteFile(hParentStdOut, chBuf,
                           dwRead, &dwWritten, NULL);


       if (! bSuccess )
       {
           cout<<"Break";
           break;
       }

    }
    cout<<endl<<chBuf;

}







int main()
{
    HANDLE hfile;
    HANDLE *hProc;//pointer to massive of descriptors

    LPSTR path1 = "C:\\Users\\Gennady\\Desktop\\WinApi\\FILE3";
    LPSTR path2 = "C:\\Users\\Gennady\\Desktop\\WinApi\\FILE3\\FILE11";
    LPSTR path3 = "C:\\Users\\Gennady\\Desktop\\WinApi\\FILE3";

    LPCSTR process1 = "C:\\Users\\Gennady\\Downloads\\Win32Lab2018\\WinApiLab3\\Process1\\bin\\Release\\Process1.exe";


    BOOL cr_p;
    STARTUPINFO si = {0};
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi = {0};

    LPSTR path = "E:\\alkelpad\\AkelPad.exe";
    if(!CreateProcess(NULL, path,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi) )
    {
        cout<<"Your CreateProcess (cr_p) ERROR!"<<GetLastError()<<endl;
    }
        int i = 11;
        CreateDirSystem(path1,i);

        CopyDirSystem(path2, path3);
        cout<<endl;
        TCHAR buffer[Size];
        DWORD bufsize = Size;
        cout<<"Your Result(GetWindowsDirectory) :"<<GetWindowsDirectory(buffer,bufsize)<<endl;//<<"Your Error: "<<GetLastError()<<endl;
        cout<<"Your Buf:"<<buffer<<endl;
        TCHAR szDir[256];
        GetCurrentDirectory (255, szDir);
        cout<<endl<<"Your Current Directory: "<<szDir<<endl;

        LPVOID enstr = GetEnvironmentStrings();
        LPSTR str = (LPSTR)enstr;
        cout<<endl<<"Your Variable Environments: "<<endl;//Your ConsoleCommand: SET;
        while (str [0] != '\0' )
        {
            str = str + strlen (str) + 1;
            cout<<str<<endl;
        }
        FreeEnvironmentStrings( (LPSTR)enstr );


    //Snapshot:
    HANDLE hs = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//(AllProcessSnapshot, RootProcess);
    if(!hs)
        cout<<"Your CreateToolhelp32Snapshot (hs) ERROR!"<<GetLastError()<<endl;
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(PROCESSENTRY32);// because structure PROCESSENTRY32 divided intp two parts for ASCII and UNICODE

    HANDLE process;//process that we will close;

    Process32First(hs,&pe);
    printf("Pr_id\t|Exe\t\t|C.Threat\n");
    do
    {
        printf("%d\t|%s\t|%d\n",pe.th32ProcessID,pe.szExeFile,pe.cntThreads);
    }while(Process32Next(hs,&pe));
    CloseHandle(hs);

    if(process)
    {
        TerminateProcess(process,0);
        CloseHandle(process);
    }

    SECURITY_ATTRIBUTES sa;//FOR INHERITNCE DESCRIPTOR
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;//point to SecurityDescriptor address
    sa.bInheritHandle = TRUE;//determine if your descriptor is inherited

    if ( ! CreatePipe(&CHILD_OUT_R, &CHILD_OUT_W, &sa, 0) )
        cout<<"Error CreatePipe(&CHILD_OUT_R, &CHILD_OUT_W, &sa, 0)";
    if ( ! SetHandleInformation(CHILD_OUT_R, HANDLE_FLAG_INHERIT, 0) )
      cout<<"Error SetHandleInformation(CHILD_OUT_R, HANDLE_FLAG_INHERIT, 0)";

    if (! CreatePipe(&CHILD_IN_R, &CHILD_IN_W, &sa, 0))
      cout<<"Error CreatePipe(&CHILD_IN_R, &CHILD_IN_W, &sa, 0)";

    if ( ! SetHandleInformation(CHILD_IN_W, HANDLE_FLAG_INHERIT, 0) )
      cout<<"Error SetHandleInformation(CHILD_IN_W, HANDLE_FLAG_INHERIT, 0)";

    STARTUPINFO si1 = {0};
    PROCESS_INFORMATION pi1 = {0};
    ChildProcess(process1,"C:\\",si1,pi1);

    LPCSTR pathfile = "C:\\Users\\Gennady\\Desktop\\WinApi\\FILE3\\l4.txt";
    inputfile = CreateFile(
       pathfile,
       GENERIC_READ,
       0,
       NULL,
       OPEN_EXISTING,
       FILE_ATTRIBUTE_READONLY,
       NULL);

    WriteToPipe();

    ReadFromPipe();



    //cout<<getenv("path")<<endl;//We can check our old/new ENVIRONMENT

    //Pipe for child stdout:
    //BOOL pipe = CreatePipe(&ph_out_r,&ph_out_w,BufferSize,&sa,0);//PHANDLE,PHANDLE,nSIZE,DWORD
    //if(pipe == FALSE)


    //ChildProcess(process1,"C:\\",si1,pi1);
    return 0;
}
