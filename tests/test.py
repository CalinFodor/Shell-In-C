import os
import subprocess

EXECUTABLE = "./shell"


def command_output_test(test_input: str, expected_output: str):
    print(f"Testing command: {repr(test_input)}")
    print(f"Expecting: {repr(expected_output)}")

    if not test_input.endswith("\n"):
        test_input += "\n"
    

    proc = subprocess.Popen(
        [EXECUTABLE, "--test"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    try:
        stdout_data, stderr_data = proc.communicate(input=test_input, timeout=10)
    except subprocess.TimeoutExpired:
        proc.kill()
        print("Test FAILED! (Timeout)")
        return

    actual_output = stdout_data.removesuffix("\n")
    
    try:
        assert proc.returncode == 0
        assert expected_output == actual_output
        print("Test PASSED!")
    except AssertionError:
        print("Test FAILED!")
        print(f"Got: {repr(actual_output)}")
        print(f"Stderr: {repr(stderr_data.strip())}")
        print(f"Return code: {proc.returncode}")
    finally:
        print()


if __name__ == "__main__":
    command_output_test("echo hello", "hello")
    command_output_test("pwd", os.getcwd())
    command_output_test("cat main.c | grep main | wc -l", "1")

    command_output_test("cd / && pwd", "/")
    command_output_test("set FOO bar\necho $FOO", "bar")
    command_output_test("cd /nonexistent_dir_12345", "")
    command_output_test("cd && pwd", os.path.expanduser("~"))

    command_output_test("echo 'hello   world'", "hello   world")
    command_output_test("   echo     spaced   ", "spaced")
    command_output_test('echo ""', "")

    command_output_test(
        "echo test > /tmp/shell_test.txt\ncat /tmp/shell_test.txt", "test"
    )
    
    command_output_test("echo a > /tmp/t.txt\necho b >> /tmp/t.txt\ncat /tmp/t.txt", "a\nb")
    
    command_output_test("cat /dev/null | cat | wc -c", "0")
    command_output_test("head -c 1000000 /dev/zero | wc -c", "1000000")
    
    command_output_test("", "")
    command_output_test("   ", "")
    command_output_test("nonexistentcommand12345", "")

    if os.path.exists("/tmp/shell_test.txt"):
        os.remove("/tmp/shell_test.txt")
        