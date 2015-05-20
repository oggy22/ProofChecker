namespace ProofCheckerEditor
{
	enum Tokens
	{
//Braces
L_BRACE, R_BRACE,
L_CURLY_BRACE, R_CURLY_BRACE,

// Logical operations
AND, OR,

// Set operations and quantifiers
SET,
COMMA, SUBSET, PARTIAL, UNION, INTERSECTION,
ELEMENT, CARTESIAN_PRODUCT,
FOR_EACH, EXISTS,

// Equal
EQUAL, NEQUAL,

BIN_OPERATION, PRIME,
ID, ID_LIST,
COLUMN, ARROW,
IS, NAME,
NEW_LINE,
END_OF_FILE

	}
}
