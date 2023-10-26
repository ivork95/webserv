#ifndef PERMISSIONS_HPP
#define PERMISSIONS_HPP

#define OWREAD std::filesystem::perms::owner_read
#define OWWRITE std::filesystem::perms::owner_write
#define OWEXEC std::filesystem::perms::owner_exec

#define OTREAD std::filesystem::perms::others_read
#define OTWRITE std::filesystem::perms::others_write
#define OTEXEC std::filesystem::perms::others_exec

#define GRREAD std::filesystem::perms::group_read
#define GRWRITE std::filesystem::perms::group_write
#define GREXEC std::filesystem::perms::group_exec

#endif