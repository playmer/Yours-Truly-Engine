$fileNames = Get-ChildItem -Path "YTE" -Recurse -Include *.cpp, *.c, *.hpp, *.h, *.inl

foreach ($fileName in $fileNames) 
{
  clang-format -i $fileName.FullName;
}