file="CHANGELOG.md"
prefix=" - "
cd ..
[[ -f "$file" ]] && rm -f "$file"
git grep -l NOTE -- './*' ':(exclude)misc/note.sh' | xargs -n1 git blame -f -n -w | grep "(smzb)" | grep NOTE | sed "s/.\{9\}//" | sed "s/(.*)[[:space:]]*//" >> "$file"
while read -r line
do
    echo "${prefix}$line"
done <$file > newfile
mv newfile $file
cd misc/
