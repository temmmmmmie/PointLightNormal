del /Q /s  ".\External\Include\Source\"
xcopy /s /y /exclude:exclude_list.txt ".\Project\Source\*.h" ".\External\Include\Source\"