code Main

  -- OS Class: Project 3
  --
  -- <ROHAN PATEL>
  --

-----------------------------  Main  ---------------------------------

  function main ()
     print ("Example Thread-based Programs...\n")

      InitializeScheduler ()

      -----  Uncomment any one of the following to perform the desired test  -----

      --SleepingBarber ()
      GamingParlor()
      

      ThreadFinish ()
    endFunction



-----------------------------  SleepingBarber  ---------------------------------

enum Enter, Sit, HairBegin, HairFinish, Leave       
 
enum Sleep,Start,End     


const
    CHAIRS = 5         -- number of waiting chairs
    CUSTOMERS = 10     -- number of customers
var 
   SBArray: array [CUSTOMERS+1] of Thread = new array of Thread { CUSTOMERS+1 of new Thread }  -- Array : number of Customer_threads + 1 Barber_Thread

   customer: Semaphore = new Semaphore
   barber: Semaphore = new Semaphore
   mutex: Semaphore = new Semaphore
   waiting: int = 0

   b_Status: int = Sleep      	-- Barber Status    
   c_Status: int = Enter		    -- Customer Status

function SleepingBarber ()
	
  var i:int j:int

   customer.Init(0)		-- Initialize
   barber.Init(0)
   mutex.Init(1)

  -- print ("XXXXX BARBER  1  2  3  4  5  6  7  8  9  10")

  print("      BARBER   ")
  for j = 1 to CUSTOMERS 	-- Loop to print Customer Header 
        printInt(j)			-- print customer number
        print("   ")
  endFor

   nl()


   SBArray[0].Init("")
   SBArray[0].Fork(Barber, 0)


   for i = 1 to CUSTOMERS
      SBArray[i].Init("")
      SBArray[i].Fork(Customer, i)
   endFor   

endFunction   

function Barber (c:int)
	
	while true
		customer.Down()         -- Barber goes to sleep
		mutex.Down()
		waiting = waiting - 1   -- decrement waiting chair
		barber.Up()
		mutex.Up()
		Cut_Hair(c)
	endWhile

endFunction

function Customer (c:int)
	
	 mutex.Down()
	 CustomerStatus (c, Enter)     --update customer status to ENTER_SHOP

	 if(waiting < CHAIRS)
		waiting = waiting + 1
	    CustomerStatus (c, Sit)   --update customer status to SIT
		customer.Up()               -- Barber awake signal
		mutex.Up()
		barber.Down()
		Get_HairCut(c)
	 else 
		mutex.Up()
	 endIf

  mutex.Down()
  CustomerStatus (c, Leave)     --update customer status to Leave
  mutex.Up()

endFunction

function Cut_Hair (c:int)
    var i:int
    mutex.Down()
    BarberStatus (Start)      --update barber status to START
    mutex.Up()

    for i = 1 to 100          --Haircut waiting Busy loop
      currentThread.Yield()
    endFor
    
    while c_Status != HairFinish     -- wait until customer Haircutf
      mutex.Down()
      currentThread.Yield()
      mutex.Up()
    endWhile 

    mutex.Down()
    BarberStatus (End)       --update barber status to END
    mutex.Up()
endFunction

function Get_HairCut (c:int)
    
    var i : int
    mutex.Down()

    while b_Status != Start   -- wait until Barber Start
        mutex.Down()
        currentThread.Yield()
       mutex.Up()
    endWhile

    CustomerStatus (c, HairBegin) --update customer status to HaircutBegin
    mutex.Up()

    for i = 1 to 100           --Haircut waiting Busy loop
      currentThread.Yield()
    endFor

    mutex.Down()
    CustomerStatus (c, HairFinish)  --update customer status to HaircutFinish
    mutex.Up()

    
    while b_Status != End     -- wait until Barber End
        mutex.Down()
        currentThread.Yield()
        mutex.Up()
    endWhile

    --mutex.Down()
    --CustomerStatus (c, Leave)
    --mutex.Up()

endFunction

function ChairStatus()    -- function for printing chair status
    var i:int
    
    for i = 0 to CHAIRS - 1
      if waiting > i
          print("X")      -- for occupide chair
      else
          print("-")      -- for available chair
      endIf      
    endFor  
endFunction

function CustomerStatus (c:int , status: int)     -- function for printing customer status
  var i:int

  c_Status = status     
  ChairStatus()           -- printing chair status before  customer status
  -- printInt(status)
  print ("     ")
  for i = 0 to c - 1      -- loop for space management
    print ("    ")
  endFor

  switch c_Status
    case Enter:
      print (" E")
      break
    case Sit:
      print (" S")
      break
    case HairBegin:
      print (" B")
      break
    case HairFinish:
      print (" F")
      break 
    case Leave:
      print (" L")
      break
  endSwitch     

  nl()  

endFunction


function BarberStatus (status:int )   -- function for printing Barber status
    
    b_Status= status      -- status is assign to enum

    ChairStatus()         -- printing chair status before Barber status
    --printInt(status)

    switch b_Status
      case Sleep:
        print("  ")
        break
      case Start:
        print(" start")   
        break
      case End:  
        print(" end")   
        break
    endSwitch    

    nl()

endFunction



------------------------------------------- GAMING PARLOR --------------------------------------------

const 
  TOTAL_DICE = 5      -- Available number of Dice at front desk 
  TOTAL_GROUP = 8     -- Available groups that will play games


var
  monitor: GameParlor 
  mLock: Mutex = new Mutex             
  monitorLock: Mutex = new Mutex 
  group: array[8] of Thread = new array of Thread {8 of new Thread}


function GamingParlor ()

  monitor = new GameParlor
  monitor.Init()

  group[0].Init ("A")
  group[0].Fork (StartGame,4)

  group[1].Init ("B")
  group[1].Fork (StartGame,4)

  group[2].Init ("C")
  group[2].Fork (StartGame,5)

  group[3].Init ("D")
  group[3].Fork (StartGame,5)

  group[4].Init ("E")
  group[4].Fork (StartGame,2)

  group[5].Init ("F")
  group[5].Fork (StartGame,2)

  group[6].Init ("G")
  group[6].Fork (StartGame,1)

  group[7].Init ("H")
  group[7].Fork (StartGame,1)

endFunction


function StartGame (req_Dice: int)

  var
     i: int

  for i = 1 to 5
    monitor.Request(req_Dice)
    monitor.PlayGame()
    monitor.Return(req_Dice)   
  endFor

endFunction



class GameParlor

  superclass Object

  fields
    available_Dice: int             -- indicate available number of Dice at front desk
    wait_Group: int                 -- indicate number of group waiting for Dice
    cond_waiting: Condition         -- Condition variable for waiting group
    cond_current: Condition         -- Condition variable for current Thread waiting for req_Dice

  methods
    Init()
    Request(req_Dice: int)
    PlayGame()
    Return(req_Dice: int)
    Print(str: String, count: int)

endClass  


behavior GameParlor

    method Init()

      
      mLock.Init()
      monitorLock.Init()
      available_Dice = 8
      wait_Group = 0
      cond_waiting = new Condition
      cond_current = new Condition
      cond_waiting.Init()
      cond_current.Init()

    endMethod


    method Request(req_Dice: int)
        
      monitorLock.Lock()                              -- monitor Lock

        self.Print("requests", req_Dice )             -- print req_Dice
        
        wait_Group = wait_Group + 1                   -- increment waiting group
        if wait_Group > 1                             
          cond_waiting.Wait(&monitorLock)             -- add in waiting group if current thread is waiting for req_Dice
        endIf

        if available_Dice < req_Dice
          cond_current.Wait(&monitorLock)             -- wait until req_Dice available
        endIf 

        mLock.Lock()
        available_Dice = available_Dice - req_Dice    -- reduce total number of dice at front desk
        mLock.Unlock()

        wait_Group = wait_Group - 1                   -- decrement waiting group
        cond_waiting.Signal(&monitorLock)             -- wake up from waiting group 

        self.Print("proceeds with", req_Dice )        -- print processed dice

      monitorLock.Unlock()                            -- monitor Unlock

    endMethod



    method PlayGame()

      var i : int
      for i = 1 to 3
        currentThread.Yield()
      endFor

    endMethod


    method Return(req_Dice: int)

      monitorLock.Lock()

        mLock.Lock()
        available_Dice = available_Dice + req_Dice            -- Add dice to available dice at front desk
        mLock.Unlock()
        self.Print("releases and adds back", req_Dice )       -- print released dice
        cond_current.Signal(&monitorLock)                     -- wake up current wait_Group

      monitorLock.Unlock()

    endMethod


    method Print (str: String, count: int)
        --
        -- This method prints the current thread's name and the arguments.
        -- It also prints the current number of dice available.
        --
        print (currentThread.name)
        print (" ")
        print (str)
        print (" ")
        printInt (count)
        nl ()
        print ("------------------------------Number of dice now available = ")
        printInt (available_Dice)
        nl ()
    endMethod

endBehavior


endCode















