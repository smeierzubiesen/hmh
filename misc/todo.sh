cd ..
file="todo.txt"
[[ -f "$file" ]] && rm -f "$file"
git grep -l TODO | xargs -n1 git blame -f -n -w | grep "smzb" | grep TODO | sed "s/.\{9\}//" | sed "s/(.*)[[:space:]]*//" >> "$file"
cd misc/
