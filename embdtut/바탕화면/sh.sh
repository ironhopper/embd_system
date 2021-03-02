#! \bin\bash
for i in 1 2 3 4 5 # go loops five times
do
	if [ $i -le 3 ] # ps.txt append when i values are 1 2 3
	then
		echo "$i" >> ps.txt # results append ps.txt with number
		ps >> ps.txt
	fi

	echo "$i" >> ls.txt # results append ls.txt with number
	ls >> ls.txt
done
