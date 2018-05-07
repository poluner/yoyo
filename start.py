import subprocess

is_running = False
processes = subprocess.check_output(["ps", "-ef"]).decode()
for p in processes.split('\n'):
    if "yoyo_sniffer" in p:
        is_running = True


if is_running:
    subprocess.call(
        "(/data/apps/go/src/github.com/LiuRoy/yoyo/processes/sniffer/yoyo_sniffer &)",
        shell=True,
        cwd="/data/apps/go/src/github.com/LiuRoy/yoyo/processes/sniffer/"
    )
