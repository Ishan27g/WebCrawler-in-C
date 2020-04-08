name="submission"

at_time="$(date)"

echo "pushing comming with comment : $name at $at_time"
git commit -m "$name"
git push origin master
