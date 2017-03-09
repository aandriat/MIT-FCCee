#!/bin/bash

# This bash script runs n-1_tree_hist and n1_draw together in an n-1 sample cutting configuration. Before using, modify n-1_tree_hist to work with the samples you have generated with your prefered cuts and filename, and modigy n1_draw to match

folder=/afs/cern.ch/work/a/aandriat/public/jet_algs/antikt_inclusive  # Location of generation directory - do not change after running setup.sh

function back {
    cd $folder
}
back

cut_to_make=12

echo "Starting n-1 analysis"

rm -rf inverse_n-1
mkdir -p inverse_n-1

for i in `seq 1 $cut_to_make`;
	do
		echo "Cut $i"
		cp input/n1_tree_hist.C inverse_n-1/
		cp input/n1_draw.C inverse_n-1/
		back

		cd inverse_n-1
		sed -i 's:SSS:'$i':g'  n1_tree_hist.C
		sed -i 's:SSS:'$i':g'  n1_draw.C

		if [ "$i" -eq 1 ]; then
			sed -i 's:QQQ:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:QQQ:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 2 ]; then
			sed -i 's:WWW:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:WWW:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 3 ]; then
			sed -i 's:EEE:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:EEE:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 4 ]; then
			sed -i 's:RRR:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:RRR:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 5 ]; then	
			sed -i 's:TTT:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:TTT:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 6 ]; then
			sed -i 's:YYY:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:YYY:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 7 ]; then
			sed -i 's:UUU:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:UUU:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 8 ]; then
			sed -i 's:III:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:III:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 9 ]; then
			sed -i 's:OOO:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:OOO:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 10 ]; then
			sed -i 's:PPP:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:PPP:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 11 ]; then
			sed -i 's:LLL:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:LLL:'0':g'  n1_tree_hist.C
		fi

		if [ "$i" -eq 12 ]; then
			sed -i 's:KKK:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:KKK:'0':g'  n1_tree_hist.C
		fi
		if [ "$i" -eq 13 ]; then
			sed -i 's:JJJ:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:JJJ:'0':g'  n1_tree_hist.C
		fi
		if [ "$i" -eq 14 ]; then
			sed -i 's:HHH:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:HHH:'0':g'  n1_tree_hist.C
		fi
		if [ "$i" -eq 15 ]; then
			sed -i 's:GGG:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:GGG:'0':g'  n1_tree_hist.C
		fi
		if [ "$i" -eq 16 ]; then
			sed -i 's:FFF:'1':g'  n1_tree_hist.C
		else 
			sed -i 's:FFF:'0':g'  n1_tree_hist.C
		fi


		echo "Starting n-1_tree_hist"
		root -l n1_tree_hist.C+ -q
		echo "Starting n1_draw"
		root -l n1_draw.C+ -q
		back
		echo "Complete"

    done
echo "Done"
echo
echo
echo



