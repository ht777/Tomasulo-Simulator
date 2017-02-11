// istringstream::str

#include <iostream>
#include <fstream>
#include <stdbool.h>
#include <sstream>
#include <string>
#include <stdint.h>
#include <list>

using namespace std;
struct instruction {int address; int optype;int dest_reg; int source_reg1; int source_reg2; char state; int tags;};
struct RAT {bool valid; int tag; int value;};
struct ROB {int address; int optype; int source_reg1; int source_reg2; int dest_reg; char state;
			int functional_unit; int tags; int rat_reg; ROB* next;};

struct RS{bool valid1; bool valid2; int value1; int value2; int tag1; int tag2; ROB *inst_ptr;};



struct FU1 {int sr1; int sr2; int res; int tag; bool free; int delay; };
struct FU2 {int sr1; int sr2; int res; int tag; bool free; int delay; };
struct FU3 {int sr1; int sr2; int res; int tag; bool free; int delay; };
struct FU4 {int sr1; int sr2; int res; int tag; bool free; int delay; };
struct FU5 {int sr1; int sr2; int res; int tag; bool free; int delay; };
struct FU6 {int sr1; int sr2; int res; int tag; bool free; int delay; };
int dispatch_instr_count,retire_count=0;
int cycle_count;
struct Queue{ROB *front,*end; int numInstr;};
void fetch();
void dispatch();
void execute();
void issue();
void retire();
RAT *rat = new RAT[128];
ROB* rob = new ROB[1024];
RS *rs = new RS[1024];

FU1 funct1;  FU2 funct2;  FU3 funct3;
FU4 funct4;  FU5 funct5;  FU6 funct6;
int stat_cycles;

int tag_assign = 0, schedule_queue_count=0, rs_count=0;
list <ROB*> dispatch_list, temp_list, temp1_list;
list <ROB*> issue_list,execute_list;

Queue q;
	ifstream prog ("C:/Users/thaku/Desktop/toma.txt");
	int count =4;

int main()
{
	 q.front = NULL; q.end = NULL; q.numInstr=0;
	funct1.free=funct2.free=funct3.free=true;
	funct4.free=funct5.free=funct6.free=true;
	int i=0;
	for (int j=0; j<1024; j++)
	{
		rs[j].valid1 = true;
		rs[j].valid2 = true;

	}
	for (int m=0 ; m<128; m++)
	{
		rat[m].valid = true;
	}



	while (i<30)
	{
		retire();
		execute();
		issue();
		dispatch();
		fetch();
		i++;
		stat_cycles++;
		cout << i <<endl;
		//cout << "Queue front state inside " << (q.front)->state << endl;

	}
    for (list<ROB*>::iterator it=issue_list.begin(); it != issue_list.end(); ++it)
    {

		cout << "State " << (**it).state << endl;
		cout << "Tag " << (**it).tags << endl;
		//cout << "State " << (**it).state << endl;
		//cout << "State " << (**it).state << endl;
	}
	cout << "Retire count " << retire_count << endl;
	cout << "total cycles " << cycle_count << endl;

	return 0;
}
int temp1_count=0;
void fetch()
{
		while(!prog.eof() && dispatch_list.size()<8 &&  count!=0 ) // To get you all the lines.
	{

			//count--;



			string STRINGS;
			getline(prog,STRINGS); // Saves the line in STRING.
			if (prog.eof()) break;

			cout<<STRINGS<<endl; // Prints our STRING.
			istringstream iss;
			iss.str (STRINGS);

		  //std::istringstream iss;
		  //std::string strvalues = "32 240 2 1450";

		  //iss.str (strvalues);
		 //cout << iss.str() << endl;

		 for (int i=0; i<5; i++)
		 {
			//while(iss >> word)
			//{
		/* do stuff with word */
			unsigned int word;
			iss >> hex >> word;
				  if (i==0)
				  {
						rob[tag_assign].address=word;
						cout << "Tag: " << tag_assign<< endl;
						cout << "i = " << i << "	address " << word<< endl;

				  }
				  else if (i==1)
				  {
						rob[tag_assign].optype=word;
						cout << "Tag: " << tag_assign << endl;
						cout << "i = " << i << "	optype " << word<< endl;

				  }
				  else if (i==2)
				  {
						rob[tag_assign].dest_reg=word;
						cout << "Tag: " << tag_assign<< endl;
						cout << "i = " << i << "	dest_reg " << word<< endl;
				  }
				  else if (i==3)
				  {
						rob[tag_assign].source_reg1=word;
						cout << "Tag: " << tag_assign<< endl;
						cout << "i = " << i << "	source 1 " << word<< endl;
				  }
				  else if (i==4)
				  {
						rob[tag_assign].source_reg2=word;
						rob[tag_assign].state='F';
						rob[tag_assign].tags=tag_assign;
						cout << "Tag: " << tag_assign<< endl;
						cout << "i = " << i << "	Source 2 " << word<< endl;
						if (tag_assign >0)
							rob[tag_assign-1].next = &rob[tag_assign];
						else
							rob[tag_assign].next = NULL;


						if (q.end == NULL)//queue is empty
						{
							  q.front = &rob[tag_assign];
							  q.end = &rob[tag_assign];
							  //if (rob_temp == NULL)
								//cout << "rob is null" << endl;
							  q.numInstr = 1;
						}
						else
						{
							  q.end->next = &rob[tag_assign];;
							  q.end = &rob[tag_assign];
							  q.numInstr++;

						}
						if (tag_assign == 4)
							cout << "fetch is executed" << endl;
						//rob_temp = &rob[tag_assign];
						dispatch_list.push_back(&rob[tag_assign]);
						dispatch_instr_count++;


						tag_assign++;
				}

				  else{cout<<"Invalid syntax/wrong code"<<endl;}
			}

	  //dispatch_list.push_back(inst);
	  //ROB.push_back(inst);
	}


}


void dispatch()
{



	for (list<ROB*>::iterator it=dispatch_list.begin(); it != dispatch_list.end(); ++it)
	{


	   if((**it).state=='D')
	   {

			temp_list.push_back(*it);
	   }

	}

	for (list<ROB*>::iterator it=temp_list.begin(); it != temp_list.end();)
	{

		if (schedule_queue_count!= 8)//need a value of full
		{

		  for (list<ROB*>::iterator it1=dispatch_list.begin(); it1 != dispatch_list.end();)
		  {
				if ((**it).tags == (**it1).tags)
				{
					if ((**it).tags==4)
						cout << "decode is executed" << endl;
					int source1,source2,dest;
					  (**it1).state='S';
					  issue_list.push_back(*it1);
					  source1=(**it1).source_reg1;
					  source2=(**it1).source_reg2;
					  dest=(**it1).dest_reg;

						//we need to initialize RAT
					  if (dest != -1)
						  rat[dest].valid=false;

					  rat[dest].tag=rs_count;//x
					//  cout << "RAT dest: " << dest << "tag: "<<rs_count<<endl;
					rs[rs_count].inst_ptr = *it1;

					  if (rat[source1].valid == true || source1==-1)
					  {
							rs[rs_count].valid1 =true;
							rs[rs_count].value1=rat[source1].value;
					  }
					   if (rat[source2].valid == true || source2==-1)
					  {
							rs[rs_count].valid2 =true;
							rs[rs_count].value2=rat[source2].value;//value at latter stage
					  }

					  if (rat[source1].valid == false  && source1!=-1)
					  {
							rs[rs_count].valid1 =false;
							rs[rs_count].tag1=rat[source1].tag;//--
					  }
					   if (rat[source2].valid == false  && source2!=-1)
					  {
							rs[rs_count].valid2 =false;
							rs[rs_count].tag2=rat[source2].tag;
					  }

					  //issue_list.push_back (*it1);
					  schedule_queue_count++;
					  dispatch_list.remove(*it1++);
					  //dispatch_queue_count--;
					  temp_list.remove(*it++);
					  rs_count++;
					  //cout << rs_count << endl;
					  break;
				}

			}

		}

	  else
		{
			cout << "Schedule queue is full" <<endl;
		}

	}
	int c=0;

	for (list<ROB*>::iterator it2=dispatch_list.begin(); it2 != dispatch_list.end(); ++it2)
	{
//		cout << "Dispatch list " << (*it2)->state << endl;
		cout << "C = " << c << endl;
		   if((*it2)->state =='F')
		   {
			   cout << "f to d" << endl;
			   (*it2)->state='D';
				c++;
		   }

	}


}

void issue()
{

	int a,b;

	  		  //cout << rat[1].value << endl << rat[1].value << endl;

	  //only if dispatch_list!=null


		  //cout << "A" << endl;
		  		//cout << "source reg 1: " << (**itx).source_reg1 << endl;
		  		//cout << rs[3].valid1 << endl;
		  		//cout << "source reg 2: " << (**itx).source_reg2 << endl;
		int issue_count=0;
		 for (list<ROB*>::iterator itx=issue_list.begin(); itx != issue_list.end(); ++itx)
		{
			issue_count++;
			cout << "Issue count " << issue_count<<endl;
		}


			//if (rs[m].valid1 == true && rs[m].valid2 == true)//--we need to include  something from issue list
			int i;
			for (i=0; i<10; i++)
			{
				//cout << "I: "<< i << " RS valid: valid1 " << rs[i].valid1 << " valid 2 " << rs[i].valid2 << endl;
				if (rs[i].valid1==true && rs[i].valid2 == true)
				{
						 for (list<ROB*>::iterator itx=issue_list.begin(); itx != issue_list.end(); ++itx)
							{
								if (rs[i].inst_ptr == *itx)
								{

		//			   if ((rs[m].tag1==(**itx).tags && rs[m].tag2==(**itx).source_reg2) || (rs[m].tag2==(**itx).source_reg1 && rs[m].tag1==(**itx).source_reg2))

						   // problem with (**itx.source)
									 cout << "A3" << endl;
								   //	(**itx).rat_reg = m;
										if ((**itx).tags==4)
											cout << "temp1 list is executed" << endl;
									 temp1_list.push_back(*itx);
									 temp1_count++;

									 issue_list.remove(*itx++);
									 //cout << "next itx: " << (**itx).tags <<endl;
									 schedule_queue_count--;
									 cout << schedule_queue_count << endl;
										  cout << "temp1 list " <<  temp1_count << endl;
										}
							// break;
						}
					}


				}




	  /*
	  int i,j;
	  for (i=0; i<=rs_count; i++)
	  {
		  if (rs[i].valid1==true && rs[i].valid2== true)
		  {
			  for (j=0; j<=128; j++)//rat_count
			  {
				  if (rat[j].tag == i)
				  {
					  	for (list<ROB*>::iterator itx=issue_list.begin(); itx != issue_list.end(); ++itx)
					  	{
							if ((**itx).dest_reg == j)
							{
								 temp1_list.push_back(*itx);
								 temp1_count++;
								 issue_list.remove(*itx++);
								 schedule_queue_count--;
								 cout << "biiiigggg" << endl;
							 }
						 }
					 }
				 }
			 }
		 }

				*/

	  int ii=0;
	  for (list<ROB*>::iterator it=temp1_list.begin(); it != temp1_list.end(); ++it)
	  {
		  		  ii++;

		  cout <<"II " << ii << endl;
			if (funct1.free==true || funct2.free==true || funct3.free==true ||
			   funct4.free==true || funct5.free==true || funct6.free==true )
			   {

				   a=rat[(**it).source_reg1].value;
				   b=rat[(**it).source_reg2].value;
				  int t=(**it).tags;

				  cout << "Optype: " << (**it).optype << endl;
	//  for (list<ROB*>::iterator it1=execute_list.begin(); it1 != execute_list.end(); ++it1)

		  cout << "inside funct modification" << endl;
				  switch ((**it).optype)
					{
						  case 0:
								if (funct1.free==true)
								{
									cout << "xxx" << endl;
									funct1.sr1 = a; funct1.sr2=b;
									funct1.free=false;
									funct1.delay=0; funct1.tag=t;
									(**it).state='E';
									(**it).functional_unit = 1;
									if ((**it).tags == 4)
										cout << "appending to execute list" << endl;
									execute_list.push_back(*it);
									temp1_list.remove(*it++);
									temp1_count--;
									break;
								}


						  case 1:
								if (funct2.free==true)
								{
									cout << "rrrr" << endl;
									funct2.sr1=a; funct2.sr2=b;
									funct2.free=false;
									funct2.delay=0;
									funct2.tag=t;
									(**it).state='E';
									(**it).functional_unit = 2;
									execute_list.push_back(*it);
									temp1_list.remove(*it++);
									break;
								}
								else if (funct3.free==true)
								{
									funct3.sr1=a;
									funct3.sr2=b;
									funct3.free=false;
									funct3.delay=0;
									funct3.tag=t;
									(**it).state='E';
									execute_list.push_back(*it);
									(**it).functional_unit = 3;
									temp1_list.remove(*it++);
									break;
								}


						  case 2:
							 if (funct4.free==true)
							 {
								   funct4.sr1=a;
								   funct4.sr2=b;
								   funct4.free=false;
								   funct4.delay=0;
								   funct4.tag=t;
									(**it).state='E';
									(**it).functional_unit = 4;
									execute_list.push_back(*it);
								   temp1_list.remove(*it++);
								   break;
							 }
							else if (funct5.free==true)
							{
								  funct5.sr1=a;
								  funct5.sr2=b;
								  funct5.free=false;
								  funct5.delay=0;
								  funct5.tag=t;
									(**it).state='E';
									(**it).functional_unit = 5;
									execute_list.push_back(*it);
								  temp1_list.remove(*it++);
								  break;
							}
							else if (funct6.free==true)
							{
								  funct6.sr1=a;
								  funct6.sr2=b;
								  funct6.free=false;
								  funct6.delay=0;
								  funct6.tag=t;
									(**it).state='E';
									(**it).functional_unit = 6;
									execute_list.push_back(*it);
								  temp1_list.remove(*it++);
								  break;
							}

					}

			   }
	  }

// not evaluating w.r.t functional unit availability
	  // only iterating through operands of the execute list is done
		  for (list<ROB*>::iterator it1=execute_list.begin(); it1 != execute_list.end(); ++it1)
		 {
			  switch ((**it1).functional_unit)
				{
					 case 1:
							if (funct1.free==false)
							{
								funct1.delay++;
								cout << "yyy" << endl;
								break;
							}

					  case 2:
							if (funct2.free==false)
							{
								cout << "r1112rrr" << endl;
								funct2.delay++;
								break;
							}
					  case 3:
							if (funct3.free==false)
							{
								//cout << "r1112rrr" << endl;
								funct3.delay++;
								break;
							}
					  case 4:
						 if (funct4.free==false)
						{
							  funct4.delay++;
							  break;
						}

					  case 5:
						if (funct5.free==false)
						{
							  funct5.delay++;
							  break;
						}
					  case 6:
						if (funct6.free==false)
						{
							  funct6.delay++;
							  break;

						}
						}
	  	  }

		//FU1			- type 0  latency = 1 cycle
	//FU2,FU3		- type 1  latency = 2 cycles
	//FU4,FU5,FU6 - type 2  latency = 4 cycles


}


//Execute();
void execute()
{
	int res_tag;
	int res_value;
    for (list<ROB*>::iterator it=execute_list.begin(); it != execute_list.end(); ++it)
    {
      //switch ((**it).tags)
      //{
      cout << "IT  tag: " << (**it).tags << endl;
      cout << "Funct1 tag: " << funct1.tag << endl;
      cout << "Funct2 tag: " << funct2.tag << endl;
      cout << "Funct3 tag: " << funct3.tag << endl;
      cout << "Funct1 delay: " << funct1.delay << endl;
      cout << "Funct2 delay: " << funct2.delay << endl;
      cout << "Funct3 delay: " << funct3.delay << endl;

        if ((**it).tags==funct1.tag)
			{
				if (funct1.delay>=1)
					{
						cout << "removing execute list" <<endl;
					  (**it).state = 'W';
					  execute_list.remove(*it++);
					  res_tag = funct1.tag;
					  res_value = funct1.res;
					  funct1.free = true;
					  funct1.delay=0;
					}
			}
        else if ((**it).tags== funct2.tag)
			{
				if (funct2.delay==2)
				{
					cout << "removing execute list" <<endl;
					  (**it).state = 'W';
					  execute_list.remove(*it++);
					  res_tag = funct2.tag;
					  res_value = funct2.res;
					  funct2.free = true;
					  //funct2.delay=0;
				}
			}
        else if ((**it).tags== funct3.tag)
			{
				if (funct3.delay==2)
				{
				  (**it).state = 'W';
				  execute_list.remove(*it++);
				  res_tag = funct3.tag;
				  res_value = funct3.res;
				  funct3.free = true;
				  funct3.delay=0;
				}
			}
        else if ((**it).tags== funct4.tag)
        {
				if (funct4.delay==4)
				{
				  (**it).state = 'W';
				  execute_list.remove(*it++);
				  res_tag = funct4.tag;
				  res_value = funct4.res;
				  funct4.free = true;
				  funct4.delay=0;
				}
        }
      else if ((**it).tags == funct5.tag)
      {
				if (funct5.delay==4)
				{
				  (**it).state = 'W';
				  execute_list.remove(*it++);
				  res_tag = funct5.tag;
				  res_value = funct5.res;
				  funct5.free = true;
				  funct5.delay=0;

				}
      }
        else if ((**it).tags == funct6.tag)
        {
				if (funct6.delay==4)
				{
				  (**it).state = 'W';
				  execute_list.remove(*it++);
				  res_tag = funct6.tag;
				  res_value = funct6.res;
				  funct6.free = true;
				  funct6.delay=0;
				}
        }
      //}

      for (int i1=0; i1<128; i1++)
      {
			if (rat[i1].tag == res_tag)
			{
			  rat[i1].value = res_value;
			  rat[i1].valid = true;
			}
      }
      for (int i2=0; i2 < 1024; i2++)
      {
			if (rs[i2].tag1 == res_tag)
			{
			  rs[i2].value1 = res_value;
			  rs[i2].valid1 = true;
			}
			else if (rs[i2].tag2 == res_tag)
			{
			  rs[i2].value2 = res_value;
			  rs[i2].valid2 = true;
			}
      }


	}
}

void retire()
{
	if (q.front == NULL)
	{
		cout << "No instruction to retire" << endl;
	}
	else
	{
		//cout << "inside retire" << endl;
		cout << "Queue front state " << (q.front)->state << endl;
		if ((q.front)->state == 'W')
		{
			cout << "inside retire" << endl;
			retire_count ++;
		   ROB *retinstr,*temp;
		  retinstr = q.front;
		  if (q.front == q.end)
		  {
			q.front=NULL; q.end=NULL; q.numInstr=0;
			cycle_count=stat_cycles;

		  }
		  else
		  {
			temp = (q.front)->next;
			q.numInstr--;
			q.front = temp;
		  }
		  if (retinstr!=NULL)
		  {
			delete retinstr;
		  }
		}
	}
}


/*
int main ()
{
    ifstream prog ("/home/hridayesh/Desktop/toma.txt");
    string STRINGS;

	getline(prog,STRINGS); // Saves the line in STRING.
	cout<<STRINGS<<endl; // Prints our STRING.
	istringstream iss;
	iss.str (STRINGS);

  //std::istringstream iss;
  //std::string strvalues = "32 240 2 1450";

  //iss.str (strvalues);
 //cout << iss.str() << endl;

  for (int n=0; n<5; n++)
  {
    unsigned int val;
    iss >> hex >> val;
    std::cout << val << '\n';
  }
  std::cout << "Finished writing the numbers in: ";
  std::cout << iss.str() << '\n';
  return 0;
}





#include <iostream>
#include <fstream>
#include <stdbool.h>
#include <sstream>
#include <string>
#include <stdint.h>
using namespace std;

int main()
{
	FILE *filename;

  ifstream prog ("/home/hridayesh/Desktop/toma.txt");
  string STRINGS;
	while(!prog.eof()) // To get you all the lines.
	{

				getline(prog,STRINGS); // Saves the line in STRING.
				istringstream iss;
				iss.str (STRINGS);
//				istringstream iss (STRINGS);

				cout << iss.str() << endl;
				unsigned int val;


				while(iss >> val)
				{
					cout << "Inside\n" << endl;
					iss >> hex >> val;
					std::cout << val << '\n';
				}
	}
	return 0;
}
*/
