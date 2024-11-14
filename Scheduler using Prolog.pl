%a)
week_schedule(WeekSlots,TAs,DayMax,WeekSched):- week_schedule_helper(WeekSlots,TAs,DayMax,WeekSched,[]).
week_schedule_helper([],_,_,WeekSched,WeekSched1):- reverse(WeekSched1,WeekSched).
week_schedule_helper([H|T],TAs,DayMax,WeekSched,WeekSchedAcc):- day_schedule(H,TAs,RemTAs,Assignment), 
									max_slots_per_day(Assignment,DayMax), insert2(Assignment,WeekSchedAcc,NewWeekSchedAcc),
									week_schedule_helper(T,RemTAs,DayMax,WeekSched,NewWeekSchedAcc).


%b)
day_schedule(DaySlots,TAs,RemTAs,Assignment):- day_schedule_helper(DaySlots,TAs,RemTAs,Assignment,[]).
day_schedule_helper([],RemTAs,RemTAs,Assignment,Assignment1):- reverse(Assignment1,Assignment).
day_schedule_helper([0	|T],TAs,RemTAs,Assignment,AssignmentAcc):- insert2([],AssignmentAcc,NewAssignmentAcc), day_schedule_helper(T,TAs,RemTAs,Assignment,NewAssignmentAcc).
day_schedule_helper([H|T],TAs,RemTAs,Assignment,AssignmentAcc):-
					H\=0,slot_assignment(H,TAs,RemTAs1,Assignment1), insert2(Assignment1,AssignmentAcc,NewAssignmentAcc),
					day_schedule_helper(T,RemTAs1,RemTAs,Assignment,NewAssignmentAcc).


%c)
max_slots_per_day2([],_).
max_slots_per_day2(List,Max):- getFirst(List,X), max_occur(X,List,MaxTA), MaxTA =< Max, removeFirst(List,List1), max_slots_per_day2(List1,Max).
max_slots_per_day(DaySched,Max):- append(DaySched,List), max_slots_per_day2(List,Max).
%helper to max slots
max_occur(_,[],0).
max_occur(X,[X|T],Max):- max_occur(X,T,Max1),Max is 1 + Max1 .
max_occur(X,[H|T],Max):- X\=H, max_occur(X,T,Max).
%helper to max slots
getFirst([],nil).
getFirst([H|_],H).
%helper to max slots
removeFirst([],nil).
removeFirst([H|T],T).
%helper to max slots


%d)
slot_assignment(LabsNum,TAs,RemTAs,Assignment):- slot_assignment_helper(LabsNum,TAs,RemTAs,Assignment,[]).
slot_assignment_helper(0,RemTAs,RemTAs,Assignment,Assignment).
slot_assignment_helper(LabsNum,TAs,RemTAs,Assignment,AssignmentAcc):- member(ta(Name,Max),TAs), \+member(Name,AssignmentAcc), Max>0,
															LabsNum1 is LabsNum - 1, Max1 is Max - 1, 
															insert2(Name,AssignmentAcc,NewAssignmentAcc),ta_slot_assignment(TAs,RemTAs1,Name),
															slot_assignment_helper(LabsNum1,RemTAs1,RemTAs,Assignment,NewAssignmentAcc).
%slot_assignment_helper(LabsNum,TAs,RemTAs,Assignment,AssignmentAcc):- member(ta(Name,0),TAs), member(ta(Name2,Max),TAs), 
%															\+member(Name2,AssignmentAcc), Name\=Name2, Max>0,
%															LabsNum1 is LabsNum - 1, Max1 is Max - 1, 
%															insert2(Name2,AssignmentAcc,NewAssignmentAcc),ta_slot_assignment(TAs,RemTAs1,Name2),
%															slot_assignment_helper(LabsNum1,RemTAs1,RemTAs,Assignment,NewAssignmentAcc).
%helper to slot assignment
insert2(nil,[],[]).
insert2(X,[],[X]).
insert2(X,[H|T],[X,H|T]).


%e)
ta_slot_assignment([ta(Name,Max)|T],[ta(Name,Max1)|T],Name):- Max1 is Max-1.
ta_slot_assignment([ta(Name,Max)|T],[ta(Name,Max)|T1],Name1):- ta_slot_assignment(T,T1,Name1).

