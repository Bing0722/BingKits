
#include <iostream>

#include <Windows.h>
#include <shellapi.h>

bool CopyFolder(const wchar_t* src, const wchar_t* dst)
{
    SHFILEOPSTRUCTW op;
    ZeroMemory(&op, sizeof(op));

    wchar_t from[MAX_PATH + 2] = {0};
    wchar_t to[MAX_PATH + 2]   = {0};

    size_t srcLen = wcslen(src);
    size_t dstLen = wcslen(dst);

    // 手动复制并确保双 null
    wmemcpy(from, src, srcLen);
    from[srcLen]     = L'\0'; // 第一个 null
    from[srcLen + 1] = L'\0'; // 第二个 null(双null结尾)

    wmemcpy(to, dst, dstLen);
    to[dstLen]     = L'\0';
    to[dstLen + 1] = L'\0';

    op.wFunc  = FO_COPY;
    op.pFrom  = from;
    op.pTo    = to;
    op.fFlags = FOF_NOCONFIRMATION   // 不弹出确认对话框（例如“是否覆盖文件？”）
                | FOF_NOCONFIRMMKDIR // 不确认自动创建目录，允许自动创建目标目录
                | FOF_SILENT         // 静默模式，不弹出进度对话框
        ;

    int ret = SHFileOperationW(&op);
    // 输出详细错误信息
    if (ret != 0 || op.fAnyOperationsAborted)
    {
        std::wcout << L"Error code: " << ret << std::endl;
        std::wcout << L"Source: " << src << std::endl;
        std::wcout << L"Dest: " << dst << std::endl;
        return false;
    }
    return true;
}

void Test_Copy()
{
    const wchar_t* File[2] = {L"D:\\文档", L"D:\\Scripts"};
    const wchar_t* src     = File[0];
    const wchar_t* dst     = L"\\\\127.0.0.1\\ShareFile\\File\\";

    if (CopyFolder(src, dst))
        std::cout << "Copy Successful!\n";
    else
        std::cout << "Copy Failed!\n";
}