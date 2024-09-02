import subprocess
import threading

def run_program(program, result):
    process = subprocess.Popen(program, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        output, error = process.communicate(timeout=2)
    except subprocess.TimeoutExpired:
        process.kill()
        result.append(False)
        return
    if process.returncode == 0:
        result.append(True)
    else:
        result.append(False)

def run_rawserv(result):
    run_program(["./output/server"], result)

def run_rawcli(result):
    run_program(["./output/client"], result)

if __name__ == "__main__":
    for i in range(100):
        result = []
        rawserv_thread = threading.Thread(target=run_rawserv, args=(result,))
        rawcli_thread = threading.Thread(target=run_rawcli, args=(result,))

        rawserv_thread.start()
        rawcli_thread.start()

        rawserv_thread.join()
        rawcli_thread.join()

        if not all(result):
            print(f"Error [{i+1}]")
            break
        else:
            print(f"Ok [{i+1}/{100}]")
