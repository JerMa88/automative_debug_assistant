# Flowscript Implementation

## Legend

### Shapes
  
<table>
    <tr>
        <td>Start and End</td>
        <td>
            <img src="../Report/images/startandend.png" alt="Graph representation" />
        </td>
        <td>The Start and end of the function are represented by 2 unique M shapes. MDiamond for "Start" and MSquare for "End".</td>
    </tr>
        <td>Function</td>
        <td>
            <img src="../Report/images/function.png" alt="Graph representation" />
        </td>
        <td>Each Function block is represented by an oval.</td>
    </tr>
        <td>Jobs</td>
        <td>
            <img src="../Report/images/job.png" alt="Graph representation" />
        </td>
        <td>Each job (loaded by the Shared Objects) is represented by a circle.</td>
    </tr>
        <td>Condition</td>
        <td>
            <img src="../Report/images/condition.png" alt="Graph representation" />
        </td>
        <td>Each condition block is represented by a rectangle. Either one or two edge will branch out and the label will differentiate if the comparison is true or false.</td>
    </tr>
        <td>Data Structure</td>
        <td>
            <img src="../Report/images/datastructure.png" alt="Graph representation" />
        </td>
        <td>Each data structure, in this example a map, is represented by a gray box.</td>
    </tr>
        <td>UI</td>
        <td>
            <img src="../Report/images/ui.png" alt="Graph representation" />
        </td>
        <td>User Interface.</td>
    </tr>

</table>

### Pointers

<table>
    <tr>
        <td>Progression Pointer</td>
        <td>
            <img src="../Report/images/progression.png" alt="Graph representation" />
        </td>
        <td>The progression of the program by sequence.</td>
    </tr>
        <td>Red Pointer</td>
        <td>
            <img src="../Report/images/red.png" alt="Graph representation" />
        </td>
        <td>Read pointer, in this case a function reading value from a data structure.</td>
    </tr>
        <td>Dot Pointer</td>
        <td>
            <img src="../Report/images/dotted.png" alt="Graph representation" />
        </td>
        <td>Assignment pointer, in this case a function assigning value to a data structure.</td>
    </tr>
        <td>Bold Point</td>
        <td>
            <img src="../Report/images/bold.png" alt="Graph representation" />
        </td>
        <td>Bold pointers entails dependencies. In this case OutputJob will not run until Compile Job finish. </td>
    </tr>
</table> 

### Structures
Structures combine different pointers and shapes to build a system of functionality.
<table>
    <tr>
        <td>For Loop</td>
        <td>
            <img src="../Report/images/forloop.png" alt="Graph representation" />
        </td>
        <td>A for loop can be a simple repeating iteration, or a certain contition (iteration, each statement, etc).</td>
    </tr>
        <td>While Loop</td>
        <td>
            <img src="../Report/images/whileloop.png" alt="Graph representation" />
        </td>
        <td>A while loop contains a condition statement.</td>
    </tr>
        <td>Iteration</td>
        <td>
            <img src="../Report/images/iteration.png" alt="Graph representation" />
        </td>
        <td>Create the Job that is pointed the amount of times labeled.</td>
    </tr>

</table> 

## JobSystem Program
```dot
digraph G {
    Start [shape=Mdiamond]

    node [shape=oval]
    StartorGetJobSystem
    RegisterJobType
    CreateJob
    Start->StartorGetJobSystem
    StartorGetJobSystem->CreateWorkerThread
    CreateWorkerThread->CreateWorkerThread
    CreateWorkerThread->CreateWorkerThread
    CreateWorkerThread->CreateWorkerThread
    CreateWorkerThread->RegisterJobType 

    node [shape=box,style=filled]
    RegisterJobType->"CompileJob\nParseJob\nOutputJob\nCustomJob"[color = red]
    RegisterJobType->ListJobs
    ListJobs->CreateJob

    node [style = unfilled]

    node [shape=circle];
    CompileJob
    ParseJob
    OutputJob
    CustomJob

    CreateJob->CompileJob
    CreateJob->ParseJob
    CreateJob->OutputJob
    CreateJob->CustomJob[label = "10"]

    CompileJob->ParseJob[style=bold,label="Link"]
    ParseJob->OutputJob[style=bold,label="Link"]
    
    subgraph cluster_1 {
        label="UI while loop->";
        
        style=filled
        color=lightgrey
        node [shape=box]
        "condition: running != 0"
        "if(command == \"stop\")"
        "if(command == \"destroy\")"
        "if(command == \"status\")"
        "If(command == \"finish\")"
    }
    CompileJob -> "condition: running != 0"
    ParseJob -> "condition: running != 0"
    OutputJob -> "condition: running != 0"

    node [shape=oval]
    "if(command == \"stop\")" -> End
    "if(command == \"destroy\")" -> DestroyJob-> End
    "if(command == \"status\")" -> GetJobStatus-> End
    "If(command == \"finish\")" -> FinishCompletedJob->End;

    End [shape=Msquare]
}
```