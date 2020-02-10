# Pymongo Flask Libary Clone
This libary Clone is intended to help any beignner to get started with learning Python, Flask, and MongoDB.

# Who is This For?
This project is good for any beginner whatsoever. The only real prerequisite for this project is that you understand the bare essentials of computer science (loops, if/else statements, etc.) Everything else you'll learn through the guide.

## Getting Started
Here's what you'll need to complete this project:
- A Command Prompt or Terminal (Mac and Linux users, just use your terminal. Windows users, [I strongly advise downloading the Ubuntu Subsystem found in the Windows Store. Please refrence  'https://docs.microsoft.com/en-us/windows/wsl/install-win10' for more details]. )
- Python 3(this comes pre-installed a Linux-friendly terminal.)
- The pip package manager (once again, this comes pre-installed.)
- Flask
- Postman
- MongoDB Atlas

## What is pip?
pip is a package manager for Python packages/modules - basically, at some point in time a developer wrote some code that did some specific function, and rather than making any developer who wanted to use such a function in their code have to write their own code to implement it, they just let other developers use their version. The way that you install that code and use it is through pip.

## What is Flask?
Flask is the web server framework that we will be using to run web servers locally with python.

## What is Postman?
Postman allows us to make Protcol calls to the web server that we wil eventually be using. Go to `https://www.postman.com/downloads/` and download it.

## What is MongoDB Atlas?
MongoDB is a persistent NoSQL Database that we will be using. MongoDB Atlas is Where we will be using to store our data in the cloud

### For Windows Users
If you downloaded the Windows Subsystem for Linux as was recommended in the last step, setting it up will take a few extra steps - once you've done those, you're good to go.

1. In your terminal, once it's installed, run `sudo apt-get update` .
2. Next, run `sudo apt-get install python-pip` .

Cool, now you have pip installed in your terminal. 

If your having trouble finding where your files are stored in Ubuntu, oen ubuntu and after setting up your user, type in `explorer.exe .` in which file manager should open.

You should be all set up now!


## Tutorial
### Step 1: Virtual Environments
In your terminal, create and navigate to a new folder for this project. Once you're in there, you're gonna wanna use pip to install a few packages. Python has something in built that is perfect for installing packages called a virtual enviroments.Virtual Environments are a sub version of your python install. It allows you to install and use packages without worrying about conflics with your root environment.

To Create a virtual environment, do:
`python3 -m venv <name of environment>`

To activate/enter a virtual environment, do:
`source <name of environment>/bin/activate`


### Step 2: Flask

After activating the virtual environment and pip, use the command `pip install flask`

### Step 3: Hello World!
Here, we will code a simple flask server that returns Hello world after launching the flask server.
#### 3.1 Code:
```python
from flask import Flask
app = Flask(__name__)
```
This is a basic start for importing and running flask code.

```python
from flask import Flask
app = Flask(__name__)

@app.route("/hello_world")
def hello_world():
    return "hello"
```
Adding a function "hello_world" with an app.route wrapper allows us to run a command when we go to the url "/hello_world", in this case, it just returns hellow world.
#### 3.2 Running Code:

After saving your work, we can then go to the command line and export the flask code. To do this, run `export FLASK_APP=<Server file name> && flask run` to launch.

To check if it worked, go to `http://127.0.0.1/hello_world` or `http://localhost:3000/hello_world`. You should see "Hello world" Displayed on the page. 
### Step 4: URL calls
URL calls allow us to specify the types of data transfers for the server. There are typically 4 types of calls used: 
 1) POST: allows for data to be inserted into the server
 2) GET: allows for data to be obtained from the server
 3) DELETE: allows for data to be removed from the server
 4) PUT: allows for data to be removed from the server
 
#### 4.1 POST
```python
```
#### 4.2 GET

#### 4.3 DELETE

#### 4.4 PUT 

#### 4.5 Postman

### Step 5: Persistance Storage.

### Step 6: Libarary Example



This is the first iteration of an app like this. As time progresses, you're going to learn a lot more about programming, how to more effectively manage data, more efficiently use APIs, and how to make your code easier to read and convenient to run. At any rate, you've built a really cool app that you can use to confuse some of your friends, and that's a really good place to start.