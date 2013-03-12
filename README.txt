                                 CS111 Grader
                      from Jeff Sullivan (jeffs@bu.edu)

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

If you have trouble getting things working, you can just email me or ask about
it in person. I'm just going to address some things that might come up:

+ It's not fool-proof!

  I've been using this program for a while now, and it hasn't screwed up on
  anything major, but it's still in development. Check after grading the first
  few students to make sure things are working properly, and if you run into
  a bug, feel free to tell me to fix it.

+ Initializing the program!

  Hopefully, the initialization is somewhat self-explanatory. You first select
  the part of the assignment you're grading - part I, part II, or part III (the
  style part), then you select the grading template (which Dr. Sullivan might
  provide for future assignments), and then you select the directory of the
  first student.

  If you want to make your own template, there's a fourth option to start up
  the template editor, which isn't perfect, but works kind of nicely. The
  template format is also fairly hand-editable, and there's an example of how
  it looks in the root directory, if you're feeling adventurous.

+ The interface!

  The interface is split into two parts. On the left, the program automatically
  opens (or tries to open) files submitted by the student that are relevant to
  the part you're grading. However, students often mislabel their files, so it
  will warn you if it can't find any and you'll have to open them yourself. On
  the right, you get a bunch of checkboxes built up from the template file. If
  the student's submission contains errors, you mark the corresponding boxes.

  Below the checkboxes, there's also a box labeled "Notes". Anything you put
  in here gets saved to the student's grade file directly, marked with "NOTE:".
  You can use it to comment on marginal deductions and such.

  There are also four toolbar buttons:
    Left arrow - Go to the previous student.
    Right arrow - Go to the next student.
    Floppy disk - Save the current student's grade.
    Cube - Toggle autosave. Before making a switch to a different student, the
    program automatically saves the current one's grade.

  The menu bar is mostly useless.

+ The code!

  The source code is included in the repository. It's not amazing, but if you
  have some reason to use it, go ahead. You could also build the program for
  yourself/a different platform, in which case you'd need wxWidgets 2.8, and
  a lot of luck, probably. I use CodeBlocks as my IDE, and the project file
  (grader.cbp) is included.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
