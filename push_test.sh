name="submission"

at_time="$(date)"

echo "pushing commit with comment at $at_time: $name"
git commit -m "$name"
git push origin master
