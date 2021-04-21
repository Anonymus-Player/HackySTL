#include <Path.hpp>
#include <Io.hpp>

namespace fs = hsd::filesystem;

#if defined(HSD_PLATFORM_POSIX)
static void file_status(const fs::path& p)
{
    auto file_stat = p.status();
    auto file_perms = file_stat.permissions().unwrap();

    hsd::io::print<"Name:           {}\n">(p.relative_name()                   );
    hsd::io::print<"Extension:      {}\n">(p.extension()                       );
    hsd::io::print<"Directory size: {}\n">(file_stat.size()           .unwrap());
    hsd::io::print<"Is Directory:   {}\n">(file_stat.is_directory()   .unwrap());
    hsd::io::print<"Is Symlink:     {}\n">(file_stat.is_symlink()     .unwrap());
    hsd::io::print<"Is Block file:  {}\n">(file_stat.is_block_file()  .unwrap());
    hsd::io::print<"Is Character:   {}\n">(file_stat.is_character()   .unwrap());
    hsd::io::print<"Is FIFO file:   {}\n">(file_stat.is_fifo_file()   .unwrap());
    hsd::io::print<"Is File:        {}\n">(file_stat.is_regular_file().unwrap());
    hsd::io::print<"Is Socket:      {}\n">(file_stat.is_socket()      .unwrap());
    
    puts("\nFile Permissions:");

    hsd::io::print<"Can Owner Read:             {}\n">(file_perms.can_owner_read          );
    hsd::io::print<"Can Owner Write:            {}\n">(file_perms.can_owner_write         );
    hsd::io::print<"Can Owner Execute:          {}\n">(file_perms.can_owner_exec          );
    hsd::io::print<"Can Owner Do everything:    {}\n">(file_perms.can_owner_do_everything );
    hsd::io::print<"Can Group Read:             {}\n">(file_perms.can_group_read          );
    hsd::io::print<"Can Group Write:            {}\n">(file_perms.can_group_write         );
    hsd::io::print<"Can Group Execute:          {}\n">(file_perms.can_group_exec          );
    hsd::io::print<"Can Group Do everything:    {}\n">(file_perms.can_group_do_everything );
    hsd::io::print<"Can Others Read:            {}\n">(file_perms.can_others_read         );
    hsd::io::print<"Can Others Write:           {}\n">(file_perms.can_others_write        );
    hsd::io::print<"Can Others Execute:         {}\n">(file_perms.can_others_exec         );
    hsd::io::print<"Can Others Do everything:   {}\n">(file_perms.can_others_do_everything);
    hsd::io::print<"Can Everyone Do everything: {}\n">(file_perms.can_all_do_everything   );
}

int main()
{
    fs::path p{"cpp"};

    file_status(p);
    puts("");
    file_status(p / "..");
    puts("");
    file_status(p / ".." / "..");
    puts("");

    for(const auto& path : p.list().unwrap())
    {
        file_status(path);
        puts("");
    }

    for(auto& parent : p.parents())
    {
        hsd::io::print<" \"{}\" |">(parent);
    }

    //fs::path p2{"testsrc"};
    //p2.copy("testcopy").unwrap();
    puts("");
    return 0;
}
#elif defined(HSD_PLATFORM_WINDOWS)
static void file_status(const fs::path& p)
{
    auto file_stat = p.status();
    auto file_perms = file_stat.permissions().unwrap();

    hsd::io::print<L"Name:           {}\n">(p.relative_name()                   );
    hsd::io::print<L"Extension:      {}\n">(p.extension()                       );
    hsd::io::print<L"Directory size: {}\n">(file_stat.size()           .unwrap());
    hsd::io::print<L"Is Directory:   {}\n">(file_stat.is_directory()   .unwrap());
    hsd::io::print<L"Is Symlink:     {}\n">(file_stat.is_symlink()     .unwrap());
    hsd::io::print<L"Is Block file:  {}\n">(file_stat.is_block_file()  .unwrap());
    hsd::io::print<L"Is Character:   {}\n">(file_stat.is_character()   .unwrap());
    hsd::io::print<L"Is FIFO file:   {}\n">(file_stat.is_fifo_file()   .unwrap());
    hsd::io::print<L"Is File:        {}\n">(file_stat.is_regular_file().unwrap());
    hsd::io::print<L"Is Socket:      {}\n">(file_stat.is_socket()      .unwrap());
    
    puts("\nFile Permissions:");

    hsd::io::print<L"Can Owner Read:             {}\n">(file_perms.can_owner_read          );
    hsd::io::print<L"Can Owner Write:            {}\n">(file_perms.can_owner_write         );
    hsd::io::print<L"Can Owner Execute:          {}\n">(file_perms.can_owner_exec          );
    hsd::io::print<L"Can Owner Do everything:    {}\n">(file_perms.can_owner_do_everything );
    hsd::io::print<L"Can Group Read:             {}\n">(file_perms.can_group_read          );
    hsd::io::print<L"Can Group Write:            {}\n">(file_perms.can_group_write         );
    hsd::io::print<L"Can Group Execute:          {}\n">(file_perms.can_group_exec          );
    hsd::io::print<L"Can Group Do everything:    {}\n">(file_perms.can_group_do_everything );
    hsd::io::print<L"Can Others Read:            {}\n">(file_perms.can_others_read         );
    hsd::io::print<L"Can Others Write:           {}\n">(file_perms.can_others_write        );
    hsd::io::print<L"Can Others Execute:         {}\n">(file_perms.can_others_exec         );
    hsd::io::print<L"Can Others Do everything:   {}\n">(file_perms.can_others_do_everything);
    hsd::io::print<L"Can Everyone Do everything: {}\n">(file_perms.can_all_do_everything   );
}

int main()
{
    fs::path p{L"cpp"};

    file_status(p);
    puts("");
    file_status(p / L"..");
    puts("");
    file_status(p / L".." / L"..");
    puts("");

    for(const auto& path : p.list().unwrap())
    {
        file_status(path);
        puts("");
    }

    for(auto& parent : p.parents())
    {
        hsd::io::print<L" \"{}\" |">(parent);
    }

    //fs::path p2{"testsrc"};
    //p2.copy("testcopy").unwrap();
    puts("");
    return 0;
}
#endif