#!/bin/bash

for ((m=2;m<=16;m++)); do
	echo -n "template<class T, EThreading th, class A1"
	for ((i=2;i<$m;i++)); do
		echo -n ", class A$i"
	done
	echo -n "> static TSharedPtr<T,th> MakeSharedPtr(A1 a1"
	for ((i=2;i<$m;i++)); do
		echo -n ", A$i a$i"
	done
	echo -n ") { return TSharedPtr<T,th>(new TShared<T,th>(a1"
	for ((i=2;i<$m;i++)); do
		echo -n ", a$i"
	done
	echo ")); }"
done
