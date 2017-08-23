#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <math.h>
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <queue>
#include <set>
#include <ctime>

#define Max 10000
#define title_line_number 7
#define req_time 20

using namespace std;

int evl_dis(int coord[][2], int ia, int ib)
{
    int x = coord[ia][0] - coord[ib][0];
    int y = coord[ia][1] - coord[ib][1];

    //return sqrt(x*x+y*y);
    //cout << x << " " << y << endl;
    return sqrt(pow(x,2) + pow(y,2));
}

int evl_cost(int *dis, vector <int> solution, int boundary)
{
    int sum = 0;
    int p1, p2;


    for(int i=0;i<boundary;i++)
    {
        p1 = solution[i];
        p2 = solution[(i+1)%boundary];
        sum += dis[p1+boundary*p2];
        //cout << "p1:" << p1 << endl;
        //p1 = p2;
    }
    //sum += dis[p2*boundary+solution[0]];
    //cout << solution.size() << " vs " << boundary << endl;

    return sum;
}

double cooling(double temperature)
{
    double apha = 0.999;
    return apha * temperature;
}

vector <int> optimize_ts(vector <int> unopt, int boundary, int *dis)
{
    int clost = boundary, snd_clost = 0;
    deque <vector <int> > tabulist;
    int tabulist_size = sqrt(boundary);
    int tmax = 1000;
    vector <int> best_so_far = unopt; // let best so far be initial state
    int bsf_cost = evl_cost(dis, unopt, boundary);

    tmax = tmax / boundary + 1;
    for(int t=0;t<tmax;t++)
    {
        int same_row, same_row2;
        for(int i=0;i<boundary;i++)
        {
            same_row = i*boundary;
            same_row2 = 0;
            clost = boundary;
            vector<int>::iterator loc = std::find(unopt.begin(), unopt.end(), i);
            //cout << "i and t: " << i << " " << t << endl;

            /*** filling neighbor with all other point ***/
            int tmp_bst = 0;
            int shifter, tmp;//tmp is improvement
            int cur_state;// something like clost(clost = x -loc)

            for (vector<int>::iterator x = unopt.begin(); x != unopt.end(); x++)
            {

                // evaluating neighbors
                if(x==loc)
                    continue;
                shifter = (x>loc)?1:-1;

                if(abs(x-loc) == 1)
                    continue;

                if(x+shifter != unopt.end() && x != unopt.begin())//if x is not the last element in vector
                {
                    tmp = dis[same_row + *(loc+shifter)] + dis[*x + (same_row2 = *(x+shifter)*boundary)] - \
                       dis[same_row + *x] - dis[same_row2 + *(loc+shifter)];
                }
                else
                {
                    tmp = dis[same_row + *(loc+shifter)] - dis[same_row + *x];
                }

                /*** candidate renew ***/
                if(x == unopt.begin())
                {
                    tmp_bst = tmp;
                    cur_state = x - loc;
                }
                else
                {
                    if(tmp < tmp_bst) // won't even choose it when last one bigger than this one.
                        continue;

                    /*** aspiration ***/
                    if(evl_cost(dis, unopt, boundary) - tmp < bsf_cost) //if reducing, it's better than best so far.
                    {
                        cur_state = x - loc;
                    }
                    /*** tabu test ***/
                    else
                    {
                        vector <int> chvec = unopt;// disposable, for checking vector whether in the list
                        clost = x - loc;// clost is just for temporary usage
                        vector<int>::iterator a = find(chvec.begin(), chvec.end(), i);

                        /*** do reverse ***/ //i.e. neighbor function
                        if(clost>0)
                            reverse(a+1, a+clost+1);
                        else if(clost < 0)
                            reverse(a+clost, a-1+1);

                        if(find(tabulist.begin(), tabulist.end(), chvec) == tabulist.end())
                        {
                            //not in tabu list. be the current state.
                            cur_state = clost;
                        }

                    }
                }
            }

            /*** candidate set, current state move on ***/
            /** do reverse **/ //i.e. neighbor function
            clost = cur_state;
            if(clost>0)
                reverse(loc+1, loc+clost+1);
            else if(clost < 0)
                reverse(loc+clost, loc-1+1);

            /** update tabu list **/
            if(tabulist.size() > tabulist_size)
            {
                tabulist.push_front(unopt);
                tabulist.pop_back();
                //cout << tabulist.size() << endl;
            }
            else
                tabulist.push_front(unopt);

            /** update best so far **/
            if(evl_cost(dis, unopt, boundary) - tmp < bsf_cost)
            {
                best_so_far = unopt;
                bsf_cost = evl_cost(dis, unopt, boundary);
            }
        }
    }

    return best_so_far;
}

vector <int> optimize_sa(vector <int> unopt, int boundary, int *dis)
{
    int clost = boundary, snd_clost = 0;// they are pointer for vector
    int last_rslt = 0, cur_rslt = 1;
    double tmp, deltaE;
    int t = 0;
    double temperature = 1;
    srand ( unsigned ( std::time(0) ) );

    while(true)
    {
        t++;
        for(int i=0;i<boundary;i++)
        {
            int same_row = i*boundary, same_row2=0;
            temperature = cooling(temperature);
            //cout << "temperature: " << temperature;
            clost = boundary;
            vector<int>::iterator loc = std::find(unopt.begin(), unopt.end(), i);

            if(temperature < 0.01)
            {
                //cout << "t: " << t << endl;
                return unopt;
            }

            int next = rand()%boundary;
            vector<int>::iterator x = std::find(unopt.begin(), unopt.end(), next);
            int shifter;
            //for (vector<int>::iterator x = unopt.begin(); x != unopt.end(); x++)
            //{
            if(x==loc)
                continue;
            shifter = (x>loc)?1:-1;

            if(abs(x-loc) == 1)
                continue;

            //evaluate deltaE and probability
            if(x+shifter != unopt.end() && x != unopt.begin())//if x is not the last element in vector
            {
                tmp = dis[same_row + *(loc+shifter)] + dis[*x + (same_row2 = *(x+shifter)*boundary)] - \
                   dis[same_row + *x] - dis[same_row2 + *(loc+shifter)];
                deltaE = tmp/(dis[same_row + *(loc+shifter)] + dis[*x + (same_row2 = *(x+shifter)*boundary)]);
            }
            else
            {
                tmp = dis[same_row + *(loc+shifter)] - dis[same_row + *x];
                deltaE = tmp/dis[same_row + *(loc+shifter)];
            }
            clost = x - loc;
            double p = exp(deltaE/temperature);
            //cout << ", probability: " << p << endl;

            if(tmp > 0)//this means reducing
            {
                /*** do reverse ***/ //i.e. neighbor function
                if(clost>0)
                {
                    reverse(loc+1, loc+clost+1);
                    //cout << "plus reverse\n";
                }
                else if(clost < 0)
                {
                    reverse(loc+clost, loc-1+1);
                    //cout << "minus reverse\n";
                }
            }
            else
            {
                /*** simulated annealing ***/
                if(p*RAND_MAX > rand())
                {
                    /*** do reverse ***/ //i.e. neighbor function
                    if(clost>0)
                    {
                        reverse(loc+1, loc+clost+1);
                        //cout << "plus reverse\n";
                    }
                    else if(clost < 0)
                    {
                        reverse(loc+clost, loc-1+1);
                        //cout << "minus reverse\n";
                    }
                }
            //}
            }
        }

    }
}

vector <int> optimize(vector <int> unopt, int boundary, int *dis)
{
    int clost = boundary, snd_clost = 0;// they are pointer for vector
    int last_rslt = 0, cur_rslt = 1;

    while(last_rslt != cur_rslt)//make sure there were no progress
    {
        last_rslt = cur_rslt;
        //cout << "this time: " << last_rslt << endl;

        for(int i=0;i<boundary;i++)
        {
            int same_row = i*boundary, same_row2=0;
            int dis_chng = 0, tmp;
            clost = boundary;
            vector<int>::iterator loc = std::find(unopt.begin(), unopt.end(), i);

            /*** follow vector ***/

            /*********************/
            /*** hill climbing ***/
            /*********************/
            int shifter;
            for (vector<int>::iterator x = unopt.begin(); x != unopt.end(); x++)
            {
                if(x==loc)
                    continue;
                shifter = (x>loc)?1:-1;

                if(abs(x-loc) == 1)
                    continue;

                if(x+shifter != unopt.end() && x != unopt.begin())//if x is not the last element in vector
                {
                    tmp = dis[same_row + *(loc+shifter)] + dis[*x + (same_row2 = *(x+shifter)*boundary)] - \
                       dis[same_row + *x] - dis[same_row2 + *(loc+shifter)];
                }
                else
                {
                    tmp = dis[same_row + *(loc+shifter)] - dis[same_row + *x];
                }
                if(dis_chng < tmp)
                {
                    dis_chng = tmp;
                    clost = x - loc;
                }
            }

            if(clost == boundary)
                continue;//no more short cut

            cur_rslt = evl_cost(dis, unopt, boundary);

            /*** do reverse ***/ //i.e. neighbor function
            if(clost>0)
                reverse(loc+1, loc+clost+1);
            else if(clost < 0)
                reverse(loc+clost, loc-1+1);

        }

    }

    return unopt;
}

int *evl_all_dis(int *all_dis, int boundary, int coord[][2])
{
    //static double all_dis[boundary*boundary];

    for(int i=0;i<boundary;i++)
    {
        //cout << "row: " << i << endl;
        for(int j=0;j<boundary;j++)
        {
            if(i==j)
                all_dis[i*boundary+j] = 0;
            else if(i<j)
                all_dis[i*boundary+j] = evl_dis(coord, i, j);
            else
                all_dis[i*boundary+j] = all_dis[i+boundary*j];
            //cout << all_dis[i*boundary+j] << " ";
        }
        //cout << endl;
    }
    return all_dis;

/*    just in case
    for(int i=0;i<num_eil;i++)
    {
        //cout << "row: " << i << endl;
        for(int j=0;j<num_eil;j++)
        {
            if(i==j)
                dis_eil[i*num_eil+j] = 0;
            else if(i<j)
                dis_eil[i*num_eil+j] = evl_dis(eil, i, j);
            else
                dis_eil[i*num_eil+j] = dis_eil[i+num_eil*j];

            //cout << dis_eil[i*num_eil+j] << " ";
        }

        //cout << endl;
    }*/
}

vector <int> solu_shffl(vector <int> ordered)
{
    srand ( unsigned ( std::time(0) ) );
    random_shuffle(ordered.begin(), ordered.end());

    //cout << "random permutation:" << endl;
   // print_vector(ordered);

    return ordered;
}

void print_vector(vector <int> content)
{
    for (vector<int>::iterator iter = content.begin(); iter != content.end(); ++iter)
        cout << ' ' << *iter;
    cout << endl;
}

int record_coord(fstream &fin, int record[][2])
{
    char line[100];
    int it = 0;
    double tmp;

    while(!fin.eof())
    {
        fin.getline(line, sizeof(line), '\n');
        it++;

        if(strcmp(line, "EOF") == 0)
            break;

        istringstream buf(line);
        if(it < title_line_number)
            continue;

        int index;
        buf >> index;
        index--;
        buf >> tmp;
        record[index][0] = tmp;
        buf >> tmp;
        record[index][1] = tmp;
        //cout << "coordinate " << it-7 << ": " << record[index][0] << "," << record[index][1] << endl;
    }

    //int num_eil = it -7;
    //for(int i=0;i< it-title_line_number ; i++)
        //cout << "coordinate: " << record[i][0] << ", " << record[i][1] << endl;
    return it - title_line_number;
}

int main()
{
    fstream f1, f2, f3;
    f1.open("eil51.tsp",ios::in);
    f2.open("lin105.tsp",ios::in);
    f3.open("pcb442.tsp",ios::in);

    int eil[51][2], lin[105][2], pcb[442][2];
    int trsl_eil[51] = {1, 22,8,26,31,28,3,36,35,20,2,29,21,16,50,34,\
    30,9,49,10,39,33,45,15,44,42,40,19,41,13,25,14,24,43,7,23,48,6,27,\
    51,46,12,47,18,4,17,37,5,38,11,32};
    //from http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/eil51.opt.tour
    int trsl_lin[105] = {1,2,6,7,10,11,15,103,21,22,29,30,31,32,33,28,23,20,\
    12,19,24,27,16,17,18,25,26,36,37,42,41,43,46,52,53,58,57,54,51,47,44,104,\
    40,49,45,48,50,55,56,59,105,62,63,70,69,74,75,81,73,76,80,86,79,77,72,64,\
    67,68,71,78,82,83,84,85,91,92,96,97,101,102,93,89,90,98,99,100,95,94,88,\
    87,66,65,61,60,39,38,35,34,14,13,4,5,9,8,3};
    //from http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/lin105.opt.tour
    int trsl_pcb[442] = {1,2,
3,
4,
5,
6,
7,
8,
9,
10,
11,
12,
13,
14,
15,
16,
17,
18,
19,
20,
53,
52,
51,
83,
84,
85,
381,
382,
86,
54,
21,
22,
55,
87,
378,
88,
56,
23,
24,
25,
26,
27,
28,
29,
30,
31,
32,
376,
377,
33,
65,
64,
63,
62,
61,
60,
59,
58,
57,
89,
90,
91,
92,
93,
101,
111,
123,
133,
146,
158,
169,
182,
197,
196,
195,
194,
181,
168,
157,
145,
144,
391,
132,
122,
110,
121,
385,
109,
120,
388,
131,
143,
156,
167,
180,
193,
192,
204,
216,
225,
233,
408,
409,
412,
413,
404,
217,
205,
206,
207,
208,
218,
219,
209,
198,
183,
170,
159,
147,
134,
124,
112,
436,
94,
95,
379,
96,
380,
97,
98,
384,
383,
113,
125,
135,
148,
160,
171,
184,
199,
210,
220,
226,
411,
410,
414,
237,
265,
437,
275,
423,
438,
272,
420,
268,
416,
264,
236,
263,
262,
261,
422,
419,
260,
259,
258,
257,
256,
255,
254,
253,
418,
417,
252,
251,
250,
415,
249,
248,
247,
246,
245,
244,
243,
242,
241,
407,
228,
235,
240,
267,
271,
270,
274,
277,
426,
280,
440,
308,
309,
283,
284,
310,
339,
311,
285,
286,
312,
340,
313,
287,
288,
314,
315,
316,
290,
289,
424,
421,
425,
291,
317,
318,
292,
293,
319,
320,
294,
295,
321,
322,
296,
278,
297,
323,
430,
429,
324,
298,
299,
300,
325,
326,
301,
302,
327,
328,
303,
304,
329,
330,
305,
306,
331,
332,
333,
432,
334,
307,
335,
336,
427,
337,
338,
375,
374,
373,
372,
371,
370,
369,
368,
345,
367,
366,
365,
431,
364,
363,
362,
344,
361,
360,
359,
435,
358,
357,
356,
434,
355,
354,
353,
343,
352,
351,
350,
349,
433,
348,
347,
346,
342,
341,
428,
282,
281,
279,
276,
273,
269,
266,
239,
238,
234,
227,
405,
406,
401,
400,
185,
172,
161,
149,
136,
126,
114,
103,
102,
441,
104,
115,
386,
127,
387,
389,
116,
138,
392,
152,
151,
137,
150,
162,
173,
186,
174,
396,
399,
187,
175,
211,
403,
221,
229,
212,
230,
222,
213,
200,
188,
176,
163,
393,
153,
139,
140,
128,
117,
105,
106,
107,
118,
129,
141,
154,
165,
164,
397,
177,
189,
201,
202,
402,
214,
223,
231,
232,
224,
215,
203,
190,191,398,178,179,395,394,166,155,142,390,130,119,108,439,82,
50,49,81,100,80,48,47,79,78,46,45,77,99,76,44,43,75,74,42,41,73,72,40,39,71,70,38,37,69,68,36,35,67,66,34,442};
    srand ( unsigned ( std::time(0) ) );

    /*** get coordination ***/
    int num_eil = record_coord(f1, eil);
    int num_lin = record_coord(f2, lin);
    int num_pcb = record_coord(f3, pcb);
    //cout << "number of eil:" << num_eil << endl;

    /*** distance evaluation ***/
    int dis_eil[num_eil*num_eil];
    int dis_lin[num_lin*num_lin];
    int dis_pcb[num_pcb*num_pcb];
    evl_all_dis(dis_eil, num_eil, eil);
    evl_all_dis(dis_lin, num_lin, lin);
    evl_all_dis(dis_pcb, num_pcb, pcb);


    vector<int> solu_eil, true_solu_eil, solu_lin, true_solu_lin, solu_pcb, true_solu_pcb;
    for (int i=0;i<num_eil;i++)
        solu_eil.push_back(i);

    for(int i=0;i<num_lin;i++)
        solu_lin.push_back(i);

    for(int i=0;i<num_pcb;i++)
        solu_pcb.push_back(i);

    for(int i=0;i<num_eil;i++)
        true_solu_eil.push_back(trsl_eil[i]-1);

    for(int i=0;i<num_lin;i++)
        true_solu_lin.push_back(trsl_lin[i]-1);

    for(int i=0;i<num_pcb;i++)
        true_solu_pcb.push_back(trsl_pcb[i]-1);

    int target_choice = 0, method_choice = 0;
    cout << "Please enter your target and method:\neil51:1 lin105:2 pcb442:3\n";
    cout << "Hill climbing:1 Simulated annealing:2 Tabu search:3\n";
    cin >> target_choice >> method_choice;

    time_t strtrd, edrd;
    int length = 0, true_ans;

    strtrd = clock();
    switch(target_choice)
    {
        case 1://eil51
            true_ans = evl_cost(dis_eil, true_solu_eil, num_eil);
            /***try 20 times***/
            for(int y=1;y<=req_time;y++)
            {
                cout << "-----------------------------------------------------\n";
                cout << "Iteration: " << y << ":\n";

                /*** random permutation solution ***/
                solu_eil = solu_shffl(solu_eil);

                int cur_cost = evl_cost(dis_eil, solu_eil, num_eil);
                cout << "EIL's unoptimized cost: " << cur_cost << endl;

                /*** start optimization ***/
                switch(method_choice)
                {
                case 1:
                    solu_eil = optimize(solu_eil, num_eil, dis_eil);
                    break;
                case 2:
                    solu_eil = optimize_sa(solu_eil, num_eil, dis_eil);
                    break;
                case 3:
                    solu_eil = optimize_ts(solu_eil, num_eil, dis_eil);
                    break;

                }

                int cost = evl_cost(dis_eil, solu_eil, num_eil);
                length += cost;
                cout << "EIL's after optimized: " << cost << endl;
                //print_vector(solu_eil);
            }

            cout << "EIL's true answer: " << true_ans << endl;
            //cout << "EIL's true answer: 426" << endl;
            break;

        case 2://lin105

            true_ans = evl_cost(dis_lin, true_solu_lin, num_lin);
            for(int y=1;y<=req_time;y++)
            {
                cout << "-----------------------------------------------------\n";
                cout << "Iteration: " << y << ":\n";

                /*** random permutation solution ***/
                solu_lin = solu_shffl(solu_lin);

                int cur_cost = evl_cost(dis_lin, solu_lin, num_lin);
                cout << "LIN's unoptimized cost: " << cur_cost << endl;

                /*** start optimization ***/
                switch(method_choice)
                {
                case 1:
                    solu_lin = optimize(solu_lin, num_lin, dis_lin);
                    break;
                case 2:
                    solu_lin = optimize_sa(solu_lin, num_lin, dis_lin);
                    break;
                case 3:
                    solu_lin = optimize_ts(solu_lin, num_lin, dis_lin);
                    break;

                }

                int cost = evl_cost(dis_lin, solu_lin, num_lin);
                length += cost;
                cout << "LIN's after optimized: " << cost << endl;
                //print_vector(solu_lin);
            }

            cout << "LIN's true answer: " << true_ans << endl;
            //cout << "LIN's true answer: 14379" << endl;
            break;

        case 3://pcb442

            true_ans = evl_cost(dis_pcb, true_solu_pcb, num_pcb);
            for(int y=1;y<=req_time;y++)
            {
                cout << "-----------------------------------------------------\n";
                cout << "Iteration: " << y << ":\n";

                /*** random permutation solution ***/
                solu_pcb = solu_shffl(solu_pcb);

                int cur_cost = evl_cost(dis_pcb, solu_pcb, num_pcb);
                cout << "PCB's unoptimized cost: " << cur_cost << endl;

                /*** start optimization ***/
                switch(method_choice)
                {
                case 1:
                    solu_pcb = optimize(solu_pcb, num_pcb, dis_pcb);
                    break;
                case 2:
                    solu_pcb = optimize_sa(solu_pcb, num_pcb, dis_pcb);
                    break;
                case 3:
                    solu_pcb = optimize_ts(solu_pcb, num_pcb, dis_pcb);
                    break;

                }

                int cost = evl_cost(dis_pcb, solu_pcb, num_pcb);
                length += cost;
                cout << "PCB's after optimized: " << cost << endl;
                //print_vector(solu_pcb);
                //cout << "PCB'a true answer: 50778" << endl;
            }
            cout << "PCB's true answer: " << true_ans << endl;
            break;
    }
    edrd = clock();

    cout << "-------------------------------------------------" << endl;
    cout << "Average running time: " << (double)(edrd - strtrd)/req_time/CLOCKS_PER_SEC << endl;
    cout << "Average best tour length: " << length/req_time << endl;
    cout << "Average difference with optimal: " << (length - true_ans*req_time)/req_time << endl;

    return 0;
}
