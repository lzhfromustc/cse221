echo "If you are not running this script with sudo, please use sudo to run it."
cd Memory\ Operations
gcc -O0 latency.c pagefault.c bandwidth.c main.c -o ./mem_main
sudo nice -n -20 ./mem_main

echo "Completed all Memory Operations measurement!"
echo "If you don't have python3, or you don't want the matplotlib, ctrl+C now to stop."
echo "You can use your own way to plot the results printed"
# sleep 5s

echo "Now installing matplotlib in python3, to plot the results in latency experiment..."
pip install matplotlib
python3 ./plot_latency.py