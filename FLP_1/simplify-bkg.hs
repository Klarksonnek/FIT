-- simplify-bkg
-- xnecas24
-- Klara Necasova

import Prelude
import Control.Applicative
import System.Environment
import System.IO()
import System.IO.Error
import Control.Exception
import Data.Char
import Data.List
import Data.List.Split
import qualified Data.Set as Set

-- serves for grammar saving and representation
data CFG = CFG {
    nonterminalsG :: String,
    terminalsG :: String,
    initSG :: Char,
    rulesG :: [(Char, String)]
    } deriving (Show)

-- | checks if nonterminals are valid
isNonterminal :: Char -> Bool
--isNonterminal nonterminal = Set.member nonterminal (Set.fromList ['A'..'Z'])	
isNonterminal = isUpper 

-- | checks if terminals are valid
isTerminal :: Char -> Bool
--isTerminal terminal = Set.member terminal (Set.fromList ['a'..'z'])	
isTerminal = isLower
 
-- | reads grammar from the input file or stdin and checks its correctness
readGrammar :: [String] -> CFG
readGrammar (nonterminals:terminals:initS:rules) = CFG readNonterminals readTerminals readInitS readRules   
    where
        -- checks terminals and nonterminals
        readSymbols symbols check = concat $ check $ splitStr symbols
        -- reads valid nonterminals
        readNonterminals = readSymbols nonterminals checkNonterminals
        -- reads valid terminals 
        readTerminals = readSymbols terminals checkTerminals
        -- reads valid initial symbol
        readInitS = checkInitS initS readNonterminals
        -- reads valid rules
        readRules = map readRule rules
        -- reads rule and represents it as tuple
        readRule rule = listToTuple $ splitOn "->" rule
        listToTuple [[l], r] = checkRule (l, r) readNonterminals ('#':readTerminals)
        listToTuple rule = error $ "invalid rule " ++ show (intercalate "->" rule)
        splitStr "" = []
        splitStr str = splitOn "," str
readGrammar _ = error "invalid grammar"

-- | checks nonterminals
checkNonterminals :: [String] -> [String]
checkNonterminals nonterminals = 
    if all checkNonterminal nonterminals then
        nonterminals
    else
        error $ "invalid nonterminal " ++ invalidSymbols checkNonterminal nonterminals
    where
        checkNonterminal [nonterminal] = isNonterminal nonterminal
        checkNonterminal _ = False

-- | checks terminals
checkTerminals :: [String] -> [String]
checkTerminals terminals = 
    if all checkTerminal terminals then
        terminals
    else
        error $ "invalid terminal " ++ invalidSymbols checkTerminal terminals
    where
        checkTerminal [terminal] = isTerminal terminal	
        checkTerminal _ = False

-- | finds invalid terminals or nonterminals
invalidSymbols :: (String -> Bool) -> [String] -> String 		
invalidSymbols check symbols =
    intercalate "," $ map (\s -> '"':(s++"\"")) $ filter (not . check) symbols

-- | checks initial symbol
checkInitS :: String -> String -> Char
checkInitS [initS] nonterminals  = 
    if initS `elem` nonterminals then
        initS
    else
        error $ "invalid start symbol " ++ show [initS]
checkInitS initS _ = error $ "invalid start symbol " ++ show initS

-- | checks rule
checkRule :: (Char, String) -> String -> String -> (Char, String)
checkRule rule@(l, r) nonterms extendedTerminals =
    if checkLeftSide && checkRightSide then
	    rule
    else
	    error $ "invalid rule " ++ show (writeRule rule)
    where
        checkLeftSide = l `elem` nonterms
        checkRightSide = not (null r)
	      && Set.isSubsetOf (symbolsToSet r) (symbolsToSet $ nonterms ++ extendedTerminals)

-- | writes grammar to the stdout
writeGrammar :: CFG -> String
writeGrammar cfg  = 
    unlines 
      $ writeSymbols (nonterminalsG cfg):writeSymbols (terminalsG cfg):writeSymbol (initSG cfg):writeRules (rulesG cfg)
    where
        -- converts list of symbols to string separated by commas 
        writeSymbols symbolList = intercalate "," $ map (:[]) symbolList
        writeSymbol s = [s]
        writeRules = map writeRule

-- | returns rule as a string
writeRule :: (Char, String) -> String		
writeRule (l, r) = l:'-':'>':r

-- | performs the algorithm 4.1
alg41 :: CFG -> CFG
alg41 cfg = CFG filterNonterminals (terminalsG cfg) (initSG cfg) (filterRules (rulesG cfg))
    where
        -- creates Nt union {S}    
        filterNonterminals = 
          setToSymbols 
          $ Set.insert (initSG cfg)
          -- only terminals and rules are necessary to create Nt		 
          $ setNt (symbolsToSet ('#':terminalsG cfg)) (rulesG cfg)
        -- gets rules that consist of symbols from Nt union sigma (sigma = sigma union #)
        filterRules = 
          filter (allowedRule getNt (symbolsToSet ('#':terminalsG cfg)))
        getNt = setNt (symbolsToSet ('#':terminalsG cfg)) (rulesG cfg)
        -- allowed rule consists of symbols from Nt union sigma (sigma = sigma union #)
        allowedRule nt extendedTerminals (l, r) = 
          Set.isSubsetOf (symbolsToSet (l:r)) (Set.union nt extendedTerminals)

-- | performs the algorithm 4.2
alg42 :: CFG -> CFG
alg42 cfg  = CFG (filterSymbols (nonterminalsG cfg)) (filterSymbols (terminalsG cfg)) (initSG cfg) (filterRules (initSG cfg) (rulesG cfg))
    where
        -- V intersection N, V intersection sigma
        filterSymbols symbols = 
          setToSymbols (Set.intersection (symbolsToSet symbols) (setV (initSG cfg) (rulesG cfg)))
        -- allowed rule consists of symbols from V
        filterRule setVOld (l,r) = Set.isSubsetOf (Set.fromList (l:r)) setVOld
        filterRules initS rules = filter (filterRule (setV initS rules)) rules

-- | creates Nt set (algorithm 4.1)
setNt :: Ord a => Set.Set a -> [(a, [a])] -> Set.Set a
-- at the beginning, Nt is empty
setNt = getNt Set.empty
    where                                               
        getNt setNOld setExtendedTerminals rules =
          -- if NOld == NNew, returns NOld
          if setNOld == setNNew setNOld setExtendedTerminals rules then
              setNOld
          -- if NOld /= NNew, continues in counting
          else
              getNt getNNew setExtendedTerminals rules
          where getNNew = setNNew setNOld setExtendedTerminals rules
        -- NOld union NNew
        setNNew setNOld setExtendedTerminals rules = 
          Set.union setNOld 
          $ symbolsToSet 
          $ rulesTakeLeftSide 
          $ rulesInNNew setNOld setExtendedTerminals rules
        rulesTakeLeftSide = map fst
        -- allowed rule consists of Nt union sigma
        ruleInNNew setNOld setExtendedTerminals (_, r) =
          Set.isSubsetOf (symbolsToSet r) (Set.union setNOld setExtendedTerminals)
        rulesInNNew setNOld setExtendedTerminals = 
          filter (ruleInNNew setNOld setExtendedTerminals)


-- | creates the V set (algorithm 4.2) 
setV :: Ord a => a -> [(a, [a])] -> Set.Set a
setV initS = getV (Set.fromList [initS])
    where
        getV setVOld rules = 
          -- if VOld =- VNew, returns VOld
          if setVOld == setVNew setVOld rules then
              setVOld
          -- if VOld /= VNew, continues in counting		  
          else 
              getV (setVNew setVOld rules) rules
        -- VOld union V
        setVNew setVOld rules = 
          Set.union setVOld 
          $ unionSets 
          $ rulesRightSideToSet 
          $ rulesTakeRigthSide 
          $ filterRules setVOld rules
        rulesTakeRigthSide = map snd
        -- left side of rule has to be in VOld set
        filterRule setVOld (l,_) = Set.member l setVOld
        filterRules setVOld = filter (filterRule setVOld)
        rulesRightSideToSet = map symbolsToSet
        unionSets = foldr Set.union Set.empty

-- | converts list of symbols to the set
symbolsToSet :: Ord a => [a] -> Set.Set a
symbolsToSet = Set.fromList

-- | converts set of symbols to the list
setToSymbols :: Set.Set a -> [a]
setToSymbols = Set.toList

-- | parses arguments
parseArguments :: [String] -> (Int, String)
parseArguments [option]
    | option == "-i" = (0, "stdin")
    | option == "-1" = (1, "stdin")
    | option == "-2" = (2, "stdin")
    | otherwise = error $ "invalid option " ++ show option 
parseArguments [option, file]
    | option == "-i" = (0, file)
    | option == "-1" = (1, file)
    | option == "-2" = (2, file)
    | otherwise = error $ "invalid option " ++ show option
parseArguments arguments = error $ "invalid arguments " ++ show (unwords arguments)

-- | reads content of file or content of stdin
getContent :: String -> IO String
getContent file = if file /= "stdin" then
                      readFile file
                  else 
                      getContents

-- | performs action according to set parameter
performAlg :: Int -> CFG -> String
performAlg option grammar
    | option == 0 = writeGrammar grammar
    | option == 1 = writeGrammar $ alg41 grammar
    | option == 2 = writeGrammar $ alg42 $ alg41 grammar
    | otherwise = error $ "invalid option " ++ show option 

-- | main function 
main :: IO ()
main = tryFun `catch` exceptFun

-- | try block
tryFun :: IO()
tryFun = do
    -- parses arguments
    (option, file) <- parseArguments <$> getArgs
    -- gets content of file (or stdin)
    content <- getContent file
    -- converts content of file (or stdin) to grammar representation
    let grammar = readGrammar $ lines content
    -- performs action according to set parameter and prints the result
    putStr (performAlg option grammar) 

-- | exception handler	
exceptFun :: IOError -> IO()
exceptFun e
    | isDoesNotExistError e = error "the input file does not exist"
    | otherwise = ioError e
