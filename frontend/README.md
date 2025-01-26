# Identity UI

The frontend is written in React using Vite as the bundler. The UI's goal is to showcase the tasks that the user currently has, as well as providing
a way for users to view their progress through the vector database. 

Both backend and data analysis portions of Identity will be hit from the UI, so it must ensure that the API for the backend and the vector database functionality are running at all times. 

## UI Breakdown

### Pages

All of the pages of the UI are found within frontend/src/pages/. The pages will be where each web route directs you to. 

For example, the main page of the Identity frontend will be hit from something like https://identityhostname:portno/, which would be where you get to view your profile statistics. 


<b>TaskInterface</b>: 

This is where you will be able to view all of your tasks and then see roughly the concepts which were derived from doing a semantic analysis on the vector representation of the tasks and its associated metadata. 


### Components

<b>Card</b>

The card is used to display one task. There will be N cards present on the TaskInterface page, each with some form of metadata present (such as task description, due date, etc..)


