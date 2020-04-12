package com.schau;

import java.util.Stack;

/**
 * Expression parser .. this is the biggie!
 */
public class Expression {
	/**
	 * Operator stack.
	 */
	private static Stack<Token> oprStk=new Stack<Token>();

	/**
	 * Value stack.
	 */
	private static Stack<StkVar> valStk=new Stack<StkVar>();

	/**
	 * aapp.
	 */
	private static Aapp aapp;

	/**
	 * Current token (and sometimes last token).
	 */
	private static Token token;

	/**
	 * Shift/Reduce table
	 *
	 * BIG FAT WARINNG: if you reorder this table you _must_ also
	 * update the token list in Token.java!
	 */
	private static final String[] tbl=new String[] {
	//	           11111111112
	//	 012345678901234567890
	//	               L 
	//	               ALA X
	//	         ENGGLLNONOO#
	//	 +-*/U()!QETETEDRDRR#$
		"rrssssrsssssssssssssr",		// + (0)
		"rrssssrsssssssssssssr",		// - (1)
		"rrrrssrsrrrrrrrrrrrrr",		// * (2)
		"rrrrssrsrrrrrrrrrrrrr",		// / (3)
		"rrrrssrsrrrrrrrrrrrrr",		// U (4)
		"ssssssssssssssssssss1",		// ( (5)
		"rrrrr2rrrrrrrrrrrrrrr",		// ) (6)
		"rrrrssrsrrrrrrrrrrrrr",		// ! (7)
		"rrrrssrsrrrrrrrrrrrrr",		// EQ (8)
		"rrrrssrsrrrrrrrrrrrrr",		// NE (9)
		"rrrrssrsrrrrrrrrrrrrr",		// GT (10)
		"rrrrssrsrrrrrrrrrrrrr",		// GE (11)
		"rrrrssrsrrrrrrrrrrrrr",		// LT (12)
		"rrrrssrsrrrrrrrrrrrrr",		// GT (13)
		"rrrrssrsrrrrrrrrrrrrr",		// LAND (14)
		"rrrrssrsrrrrrrrrrrrrr",		// LOR (15)
		"rrrrssrsrrrrrrrrrrrrr",		// AND (16)
		"rrrrssrsrrrrrrrrrrrrr",		// OR (17)
		"rrrrssrsrrrrrrrrrrrrr",		// XOR (18)
		"rrrrssrsrrrrrrrrrrrrr",		// ## (19)
		"ssssss3rssssssssssssa",		// $ (20)
	};

	/**
	 * Initialise the Expression parser.
	 *
	 * @param a aapp.
	 */
	public static void init(Aapp a) {
		aapp=a;
	}

	/**
	 * Reduce according to table. Operations are usually done with
	 * stack-1 as lhs and stack-0 as rhs.
	 *
	 * @param t token
	 * @exception Exception.
	 */
	private static void reduce(Token t) throws Exception {
		boolean v0b=false, v1b=false;
		int r=0;
		StkVar v0, v1;

		switch (t) {
			case ADD:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to +");
				}

				v0=valStk.pop();
				v1=valStk.pop();
				if (v0.type==StkVar.Type.INT) {
					if (v1.type==StkVar.Type.INT) {
						valStk.push(new StkVar(v1.intVal+v0.intVal));
					} else {
						valStk.push(new StkVar(v1.strVal+Integer.toString(v0.intVal)));
					}
				} else {
					if (v1.type==StkVar.Type.INT) {
						valStk.push(new StkVar(Integer.toString(v1.intVal)+v0.strVal));
					} else {
						valStk.push(new StkVar(v1.strVal+v0.strVal));
					}
				}
				break;

			case SUB:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to -");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}

				valStk.push(new StkVar(v1.intVal-v0.intVal));
				break;

			case MUL:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to *");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}

				valStk.push(new StkVar(v1.intVal*v0.intVal));
				break;

			case DIV:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to /");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				if (v0.intVal==0) {
					Tokenizer.error("Divide by zero");
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}

				valStk.push(new StkVar(v1.intVal/v0.intVal));
				break;

			case UMINUS:
				if (valStk.size()<1) {
					Tokenizer.error("No operand given to negation");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				valStk.push(new StkVar(-(v0.intVal)));
				break;

			case RPARAN:
				oprStk.pop();
				break;

			case NOT:
				if (valStk.size()<1) {
					Tokenizer.error("No operand given to !");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				if (v0.intVal==0) {
					valStk.push(new StkVar(1));
				} else {
					valStk.push(new StkVar(0));
				}
				break;

			case EQ:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to ==");
				}

				v0=valStk.pop();
				v1=valStk.pop();

				if (v0.type==v1.type) {
					if (v0.type==StkVar.Type.INT) {
						if (v0.intVal==v1.intVal) {
							r=1;
						}
					} else {
						if (v0.strVal.compareTo(v1.strVal)==0) {
							r=1;
						}
					}

					valStk.push(new StkVar(r));
				} else {
					Tokenizer.error("Can only compare ints to ints and strings to strings");
				}
				break;

			case NE:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to !=");
				}

				v0=valStk.pop();
				v1=valStk.pop();

				if (v0.type==v1.type) {
					if (v0.type==StkVar.Type.INT) {
						if (v0.intVal!=v1.intVal) {
							r=1;
						}
					} else {
						if (v0.strVal.compareTo(v1.strVal)!=0) {
							r=1;
						}
					}

					valStk.push(new StkVar(r));
				} else {
					Tokenizer.error("Can only compare ints to ints and strings to strings");
				}
				break;

			case GT:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to >");
				}

				v0=valStk.pop();
				v1=valStk.pop();

				if (v0.type==v1.type) {
					if (v0.type==StkVar.Type.INT) {
						if (v1.intVal>v0.intVal) {
							r=1;
						}
					} else {
						if (v1.strVal.compareTo(v0.strVal)==1) {
							r=1;
						}
					}

					valStk.push(new StkVar(r));
				} else {
					Tokenizer.error("Can only compare ints to ints and strings to strings");
				}
				break;

			case GE:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to >=");
				}

				v0=valStk.pop();
				v1=valStk.pop();

				if (v0.type==v1.type) {
					if (v0.type==StkVar.Type.INT) {
						if (v1.intVal>=v0.intVal) {
							r=1;
						}
					} else {
						if (v1.strVal.compareTo(v0.strVal)>=0) {
							r=1;
						}
					}

					valStk.push(new StkVar(r));
				} else {
					Tokenizer.error("Can only compare ints to ints and strings to strings");
				}
				break;

			case LT:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to <");
				}

				v0=valStk.pop();
				v1=valStk.pop();

				if (v0.type==v1.type) {
					if (v0.type==StkVar.Type.INT) {
						if (v1.intVal<v0.intVal) {
							r=1;
						}
					} else {
						if (v1.strVal.compareTo(v0.strVal)==-1) {
							r=1;
						}
					}

					valStk.push(new StkVar(r));
				} else {
					Tokenizer.error("Can only compare ints to ints and strings to strings");
				}
				break;

			case LE:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to <=");
				}

				v0=valStk.pop();
				v1=valStk.pop();

				if (v0.type==v1.type) {
					if (v0.type==StkVar.Type.INT) {
						if (v1.intVal<=v0.intVal) {
							r=1;
						}
					} else {
						if (v1.strVal.compareTo(v0.strVal)<=0) {
							r=1;
						}
					}

					valStk.push(new StkVar(r));
				} else {
					Tokenizer.error("Can only compare ints to ints and strings to strings");
				}
				break;

			case LAND:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to &&");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}
				if (v0.intVal!=0) {
					v0b=true;
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}
				if (v1.intVal!=0) {
					v1b=true;
				}

				if (v1b&&v0b) {
					valStk.push(new StkVar(1));
				} else {
					valStk.push(new StkVar(0));
				}
				break;

			case LOR:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to ||");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}
				if (v0.intVal!=0) {
					v0b=true;
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}
				if (v1.intVal!=0) {
					v1b=true;
				}

				if (v1b||v0b) {
					valStk.push(new StkVar(1));
				} else {
					valStk.push(new StkVar(0));
				}
				break;

			case AND:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to &");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}

				valStk.push(new StkVar(v1.intVal&v0.intVal));
				break;

			case OR:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to |");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}

				valStk.push(new StkVar(v1.intVal|v0.intVal));
				break;

			case XOR:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to ^");
				}

				v0=valStk.pop();
				if (v0.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v0.strVal+"')");
				}

				v1=valStk.pop();
				if (v1.type!=StkVar.Type.INT) {
					Tokenizer.error("Operand must be integer (got '"+v1.strVal+"')");
				}

				valStk.push(new StkVar(v1.intVal^v0.intVal));
				break;

			case CAT:
				if (valStk.size()<2) {
					Tokenizer.error("Only one operand given to ##");
				}

				v0=valStk.pop();
				String s0;
				if (v0.type==StkVar.Type.INT) {
					s0=Integer.toString(v0.intVal);
				} else {
					s0=v0.strVal;
				}

				v1=valStk.pop();
				String s1;
				if (v1.type==StkVar.Type.INT) {
					s1=Integer.toString(v1.intVal);
				} else {
					s1=v1.strVal;
				}

				valStk.push(new StkVar(s1+s0));
				break;
		}

		oprStk.pop();
	}

	/**
	 * Parse expression using shift/reduce table. Preload oprStk with
	 * token.
	 *
	 * @param t Token.
	 * @exception Exception.
	 */
	public static void parse(Token t) throws Exception {
		oprStk.clear();
		token=Token.EOF;
		oprStk.push(Token.EOF);

		if (t!=Token.EMPTYSYSTEMTOKEN) {
			oprStk.push(t);
		}

		valStk.clear();

		if ((token=Tokenizer.getToken())==Token.EOF) {
			Tokenizer.error("Missing expression");
		}	

		boolean run=true;

		while (run) {
			if (token==Token.STRING) {
				valStk.push(new StkVar(Tokenizer.strVal));
				token=Tokenizer.getToken();
			} else if (token==Token.VAL) {
				valStk.push(new StkVar(Tokenizer.intVal));
				token=Tokenizer.getToken();
			} else if (token==Token.ID) {
				valStk.push(CmdVariables.getStkVar(aapp, Tokenizer.strVal));
				token=Tokenizer.getToken();
			} else {
				t=oprStk.peek();
				switch (tbl[t.ordinal()].charAt(token.ordinal())) {
					case 'r':
						reduce(t);
						break;
					case 's':
						oprStk.push(token);
						token=Tokenizer.getToken();
						break;
					case 'a':
						run=false;
						break;
					case '1':
						Tokenizer.error("Missing right paranthesis");
					case '2':
						Tokenizer.error("Missing operator");
					case '3':
						Tokenizer.error("Unbalanced right paranthesis");
				}
			}
		}	

		if (valStk.size()!=1) {
			// Debug.dumpValStk();
			Tokenizer.error("Syntax error");
		}
	}

	/**
	 * Parse expression using shift/reduce table.
	 *
	 * @exception Exception.
	 */
	public static void parse() throws Exception {
		parse(Token.EMPTYSYSTEMTOKEN);
	}

	/**
	 * Return top StkVar.
	 *
	 * @return StkVar.
	 */
	public static StkVar getStkVar() {
		return valStk.pop();
	}
}
