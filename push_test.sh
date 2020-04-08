name="testing commit"

at_time="$(date)"

echo "pushing comming with comment : $name at $at_time"
git commit -m "$name at $at_time"
git push origin master
