🐚 Simple Shell 

A toy command-line interpreter built for learning, offering essential features like stream redirection, pipelining, and background execution, all wrapped in a simple editing environment.

✨ Features
- Stream Redirection 📤

  - Redirect input (stdin) and output (stdout) streams to files or between commands.

 - Pipelining 🔗

   - Chain commands together using pipes (|), passing the output of one command as input to another.
 - Background Execution 🚀

   - Execute commands in the background by appending & to your command.
  - Line Formatting ✍️

    - Enhanced line editing capabilities, including the ability to move the cursor using arrow keys and insert or modify text at any position.
 - Autocompletion on Tab 💡
    - Autocompletion by pressing Tab, helping navigate through file paths.
 - Signal Handling & Process Group Management ⚙️

   - Usage of process groups ensures that only the foreground process group receives signals like SIGINT (pressing Ctrl+C), leaving the shell itself unaffected.

🚧 Installation & Usage

```bash
    git clone https://github.com/MwahMallah/simple_shell.git
    make
    ./main
```