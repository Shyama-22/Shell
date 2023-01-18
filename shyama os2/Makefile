all:
	main

ls: ls.c
	gcc ls.c -o ls

cat: cat.c
	gcc cat.c -o cat

date: date.c
	gcc date.c -o date

rm: rm.c
	gcc rm.c -o rm

mkdir: mkdir.c
	gcc mkdir.c -o mkdir

main:  ls cat date rm mkdir
	gcc main.c -o main -g

clean:
	rm -rf ls cat date rm mkdir main 