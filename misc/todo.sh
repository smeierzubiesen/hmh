file="todo.txt"
prefix=" - [ ] "
cd ..
[[ -f "$file" ]] && rm -f "$file"
git grep -l TODO -- './*' ':(exclude)misc/todo.sh' | xargs -n1 git blame -f -n -w | grep "(smzb)" | grep TODO | sed "s/.\{9\}//" | sed "s/(.*)[[:space:]]*//" >> "$file"
while read -r line
do
 echo "${prefix}$line"
done <$file > newfile
mv newfile $file
cd misc/
