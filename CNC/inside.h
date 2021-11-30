0 BEGIN PGM inside MM
1 BLK FORM 0.1 Z X-338 Y+0 Z-4.8
2 BLK FORM 0.2 X+0 Y+226 Z+0
3 ;-------------------------------------
4 ;Tools
5 ;  #13 D=4 - ZMIN=-5.8 - ZMAX=+15 - flat end mill
6 ;  #15 D=3.5 TAPER=118deg - ZMIN=-6.852 - ZMAX=+15 - drill
7 ;  #16 D=3 TAPER=118deg - ZMIN=-6.701 - ZMAX=+15 - drill
8 ;  #17 D=1 TAPER=118deg - ZMIN=-6.1 - ZMAX=+15 - drill
9 ;-------------------------------------
10 ;
11 * - 3.5mm drill
12 M5
13 TOOL CALL 15 Z S4000
14 TOOL DEF 16
15 L M140 MB MAX
16 M3
17 L X-50.2 Y+168.6 R0 FMAX
18 L Z+15 R0 FMAX
19 CYCL DEF 32.0 TOLERANCE
20 CYCL DEF 32.1
21 CYCL DEF 200 DRILLING ~
  Q200=+5 ;SET-UP CLEARANCE ~
  Q201=-6.852 ;DEPTH ~
  Q206=500 ;FEED RATE FOR PLUNGING ~
  Q202=+6.852 ;INFEED DEPTH ~
  Q210=0 ;DWELL AT TOP ~
  Q203=+0 ;SURFACE COORDINATE ~
  Q204=+5 ;2ND SET-UP CLEARANCE ~
  Q211=0 ;DWELL AT BOTTOM
22 L FMAX M99
23 L X-99.2 FMAX M99
24 L X-127.6 Y+186.4 FMAX M99
25 L X-176.6 FMAX M99
26 L X-222.5 Y+137 FMAX M99
27 L X-268.5 FMAX M99
28 L Y+43 FMAX M99
29 L X-222.5 FMAX M99
30 L Z+15 FMAX
31 L M140 MB MAX
32 * - 3mm drill
33 M5
34 M1
35 TOOL CALL 16 Z S4000
36 TOOL DEF 17
37 L M140 MB MAX
38 M3
39 L X-106 Y+59.732 R0 FMAX
40 L Z+15 R0 FMAX
41 M9
42 CYCL DEF 200 DRILLING ~
  Q200=+5 ;SET-UP CLEARANCE ~
  Q201=-6.701 ;DEPTH ~
  Q206=500 ;FEED RATE FOR PLUNGING ~
  Q202=+6.701 ;INFEED DEPTH ~
  Q210=0 ;DWELL AT TOP ~
  Q203=+0 ;SURFACE COORDINATE ~
  Q204=+5 ;2ND SET-UP CLEARANCE ~
  Q211=0 ;DWELL AT BOTTOM
43 L FMAX M99
44 L Y+67.232 FMAX M99
45 L Z+15 FMAX
46 L M140 MB MAX
47 * - 1mm drill
48 M5
49 M1
50 TOOL CALL 17 Z S4000
51 TOOL DEF 13
52 L M140 MB MAX
53 M3
54 L X-106 Y+52.232 R0 FMAX
55 L Z+15 R0 FMAX
56 M9
57 CYCL DEF 200 DRILLING ~
  Q200=+5 ;SET-UP CLEARANCE ~
  Q201=-6.1 ;DEPTH ~
  Q206=70 ;FEED RATE FOR PLUNGING ~
  Q202=+1 ;INFEED DEPTH ~
  Q210=0 ;DWELL AT TOP ~
  Q203=+0 ;SURFACE COORDINATE ~
  Q204=+5 ;2ND SET-UP CLEARANCE ~
  Q211=0 ;DWELL AT BOTTOM
58 L FMAX M99
59 L Z+15 FMAX
60 L M140 MB MAX
61 * - USBSlot
62 M5
63 M1
64 TOOL CALL 13 Z S4000
65 TOOL DEF 15
66 L M140 MB MAX
67 M3
68 L X-166.15 Y+171.3 R0 FMAX
69 L Z+15 R0 FMAX
70 M9
71 L Z+5 FMAX
72 L Z+1 F200
73 L Z-5.7
74 L Y+171.292 Z-5.738 F450
75 L Y+171.271 Z-5.771
76 L Y+171.238 Z-5.792
77 L Y+171.2 Z-5.8
78 L Y+171.1
79 L X-166.142 Y+171.062
80 L X-166.121 Y+171.029
81 L X-166.088 Y+171.008
82 L X-166.05 Y+171
83 L X-161.3
84 L Y+171.5
85 L X-166.3
86 L Y+171
87 L X-166.05
88 L X-166.012 Y+171.008
89 L X-165.979 Y+171.029
90 L X-165.958 Y+171.062
91 L X-165.95 Y+171.1
92 L Y+171.2
93 L Y+171.238 Z-5.792
94 L Y+171.271 Z-5.771
95 L Y+171.292 Z-5.738
96 L Y+171.3 Z-5.7
97 L Z+15 FMAX
98 * - BackSlots
99 M3
100 L X-195.4 Y+70.4 R0 FMAX
101 L Z+15 R0 FMAX
102 L Z+5 FMAX
103 L Z+1 F200
104 L Z-5.6
105 CC Y+70.2 Z-5.6
106 CP IPA-90 DR- F450
107 L Y+70 Z-5.8
108 CC X-195.2 Y+70
109 CP IPA+90 DR+
110 L X-169.7 Y+69.8
111 L Y+70.8
112 L X-195.7
113 L Y+69.8
114 L X-195.2
115 CC X-195.2 Y+70
116 CP IPA+90 DR+
117 L X-195 Y+70.2
118 CC Y+70.2 Z-5.6
119 CP IPA+90 DR+
120 L Y+70.4 Z+5 FMAX
121 L X-170.3 Y+80.1 FMAX
122 L Z+1 F200
123 L Z-5.6
124 CC X-170.1 Z-5.6
125 CP IPA-90 DR- F450
126 L X-169.9 Z-5.8
127 CC X-169.9 Y+80.3
128 CP IPA+90 DR+
129 L X-169.7 Y+80.8
130 L X-195.7
131 L Y+79.8
132 L X-169.7
133 L Y+80.3
134 CC X-169.9 Y+80.3
135 CP IPA+90 DR+
136 L X-170.1 Y+80.5
137 CC X-170.1 Z-5.6
138 CP IPA+90 DR+
139 L X-170.3 Z+5 FMAX
140 L X-195.4 Y+90.4 FMAX
141 L Z+1 F200
142 L Z-5.6
143 CC Y+90.2 Z-5.6
144 CP IPA-90 DR- F450
145 L Y+90 Z-5.8
146 CC X-195.2 Y+90
147 CP IPA+90 DR+
148 L X-169.7 Y+89.8
149 L Y+90.8
150 L X-195.7
151 L Y+89.8
152 L X-195.2
153 CC X-195.2 Y+90
154 CP IPA+90 DR+
155 L X-195 Y+90.2
156 CC Y+90.2 Z-5.6
157 CP IPA+90 DR+
158 L Y+90.4 Z+5 FMAX
159 L X-195.4 Y+60.4 FMAX
160 L Z+1 F200
161 L Z-5.6
162 CC Y+60.2 Z-5.6
163 CP IPA-90 DR- F450
164 L Y+60 Z-5.8
165 CC X-195.2 Y+60
166 CP IPA+90 DR+
167 L X-169.7 Y+59.8
168 L Y+60.8
169 L X-195.7
170 L Y+59.8
171 L X-195.2
172 CC X-195.2 Y+60
173 CP IPA+90 DR+
174 L X-195 Y+60.2
175 CC Y+60.2 Z-5.6
176 CP IPA+90 DR+
177 L Y+60.4 Z+5 FMAX
178 L X-170.3 Y+50.1 FMAX
179 L Z+1 F200
180 L Z-5.6
181 CC X-170.1 Z-5.6
182 CP IPA-90 DR- F450
183 L X-169.9 Z-5.8
184 CC X-169.9 Y+50.3
185 CP IPA+90 DR+
186 L X-169.7 Y+50.8
187 L X-195.7
188 L Y+49.8
189 L X-169.7
190 L Y+50.3
191 CC X-169.9 Y+50.3
192 CP IPA+90 DR+
193 L X-170.1 Y+50.5
194 CC X-170.1 Z-5.6
195 CP IPA+90 DR+
196 L X-170.3 Z+15 FMAX
197 * - FrontInnerSlot
198 M3
199 L X-106.1 Y+57.032 R0 FMAX
200 L Z+15 R0 FMAX
201 L Z+5 FMAX
202 L Z+1 F200
203 L Z-1.8
204 CC X-105.9 Z-1.8
205 CP IPA-90 DR- F450
206 L X-105.7 Z-2
207 CC X-105.7 Y+57.232
208 CP IPA+90 DR+
209 L X-105.5 Y+131
210 L X-106.5
211 L Y+56.732
212 L X-105.5
213 L Y+57.232
214 CC X-105.7 Y+57.232
215 CP IPA+90 DR+
216 L X-105.9 Y+57.432
217 CC X-105.9 Z-1.8
218 CP IPA+90 DR+
219 L X-106.1 Z+15 FMAX
220 * - FrontCutout
221 M3
222 L X-56.68 Y+72.352 R0 FMAX
223 L Z+15 R0 FMAX
224 L Z+5 FMAX
225 L Z+1 F200
226 L Z-5.4
227 CC X-56.28 Z-5.4
228 CP IPA-90 DR- F450
229 L X-55.88 Z-5.8
230 CC X-55.88 Y+72.752
231 CP IPA+90 DR+
232 L X-55.48 Y+128.768
233 L X-98.52
234 L Y+51.232
235 L X-55.48
236 L Y+72.752
237 CC X-55.88 Y+72.752
238 CP IPA+90 DR+
239 L X-56.28 Y+73.152
240 CC X-56.28 Z-5.4
241 CP IPA+90 DR+
242 L X-56.68 Z+15 FMAX
243 M5
244 L M140 MB MAX
245 M30
246 END PGM inside MM
