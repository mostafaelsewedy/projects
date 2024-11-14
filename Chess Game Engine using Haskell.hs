import Data.Char
import Data.List

type Location = (Char, Int)
data Player = White | Black deriving (Show, Eq)
data Piece = P Location | N Location | K Location | Q Location | R Location | B Location deriving (Show, Eq)
type Board = (Player, [Piece], [Piece])

setBoard :: Board
setBoard = (White, whitePieces, blackPieces)
  where
    whitePieces = [R ('a', 1), N ('b', 1), B ('c', 1), Q ('d', 1), K ('e', 1), B ('f', 1), N ('g', 1), R ('h', 1)]
                   ++ replicate 8 (P ('a', 2))
    blackPieces = [R ('a', 8), N ('b', 8), B ('c', 8), Q ('d', 8), K ('e', 8), B ('f', 8), N ('g', 8), R ('h', 8)]
                   ++ replicate 8 (P ('a', 7))
				
				
visualizeBoard :: Board -> String
visualizeBoard (player, whitePieces, blackPieces) =
  unlines (header : rows ++ [turn])
  where
    header = " " ++ unwords (map (\c -> " " ++ [c] ++ " ") columnLetters)
    divider = replicate (length header) '-'
    rows = [showRow row whitePieces blackPieces (player,whitePieces,blackPieces) | row <- reverse [1..8]]
    allPieces = whitePieces ++ blackPieces
    turn = "Turn: " ++ show player
	
showSquare :: Maybe Piece -> Board -> String
showSquare maybePiece board = case maybePiece of
  (Just (P loc)) -> "P" ++ pieceSuffix (P loc) board
  (Just (N loc)) -> "N" ++ pieceSuffix (N loc) board
  (Just (K loc)) -> "K" ++ pieceSuffix (K loc) board 
  (Just (Q loc)) -> "Q" ++ pieceSuffix (Q loc) board 
  (Just (R loc)) -> "R" ++ pieceSuffix (R loc) board 
  (Just (B loc)) -> "B" ++ pieceSuffix (B loc) board
  Nothing -> "  "


pieceSuffix :: Piece -> Board -> String
pieceSuffix (P (col, row)) (_, whitePieces, blackPieces) = if ((P (col,row)) `elem` whitePieces) then "W" else "B"
pieceSuffix (N (col, row)) (_, whitePieces, blackPieces) = if ((N (col,row)) `elem` whitePieces) then "W" else "B"
pieceSuffix (K (col, row)) (_, whitePieces, blackPieces) = if ((K (col,row)) `elem` whitePieces) then "W" else "B"
pieceSuffix (Q (col, row)) (_, whitePieces, blackPieces) = if ((Q (col,row)) `elem` whitePieces) then "W" else "B"
pieceSuffix (R (col, row)) (_, whitePieces, blackPieces) = if ((R (col,row)) `elem` whitePieces) then "W" else "B"
pieceSuffix (B (col, row)) (_, whitePieces, blackPieces) = if ((B (col,row)) `elem` whitePieces) then "W" else "B"

-- Define the letters for the column headers
columnLetters :: String
columnLetters = "abcdefgh"

-- Define a function to visualize a single row on the board
showRow :: Int -> [Piece] -> [Piece] -> Board -> String
showRow row whitePieces blackPieces board =
  "|" ++ concatMap (\col -> (showSquare (find (\p -> getLocation p == (col, row)) allPieces)) board) columnLetters
  ++ "|" ++ show row
  where
    allPieces = whitePieces ++ blackPieces
				
isLegal :: Piece -> Board -> Location -> Bool
isLegal piece board destination = isValidDestination destination && isPieceAllowedToMove && (not (destination `elem` (pieceLocs currentPieces)))
  where
    (currentPieces,currentPlayer, otherPlayerPieces) = if getCurrentPlayer board == White then (getWhitePieces board, White, getBlackPieces board) else (getBlackPieces board, Black, getWhitePieces board)
    isPieceAllowedToMove = case piece of
      P loc -> isPawnMove loc destination currentPlayer ((pieceLocs currentPieces)++(pieceLocs otherPlayerPieces))
      N loc -> isKnightMove loc destination
      B loc -> isBishopMove loc destination
      R loc -> isRookMove loc destination
      Q loc -> isQueenMove loc destination
      K loc -> isKingMove loc destination
    isValidDestination (col, row) = col `elem` ['a'..'h'] && row `elem` [1..8]
	
-- Helper functions for movement rules (sample implementation)
isPawnMove :: Location -> Location -> Player -> [Location] -> Bool
isPawnMove (col,7) (col', a) Black locs = (col==col')&& (((a==6) && (not ( (col, 6) `elem` locs)))||((a==5) && (not ( (col, 5) `elem` locs))&& (not ( (col, 6) `elem` locs))))
isPawnMove (col,2) (col', a) White locs = (col==col')&& (((a==3) && (not ( (col, 3) `elem` locs)))||((a==4) && (not ( (col, 4) `elem` locs)) && (not ( (col, 3) `elem` locs))))
isPawnMove (col, row) (col', row') Black locs = col == col' && row' == row - 1 && (not((col, row-1) `elem` locs))
isPawnMove (col, row) (col', row') White locs = col == col' && row' == row + 1 && (not((col, row+1) `elem` locs))

isKnightMove :: Location -> Location -> Bool
isKnightMove (col, row) (col', row') = (abs (colDiff * rowDiff) == 2) && (abs colDiff + abs rowDiff == 3)
  where
    colDiff = ord col - ord col'
    rowDiff = row - row'

isBishopMove :: Location -> Location -> Bool
isBishopMove (col, row) (col', row') = abs (colDiff) == abs (rowDiff)
  where
    colDiff = ord col - ord col'
    rowDiff = row - row'

isRookMove :: Location -> Location -> Bool
isRookMove (col, row) (col', row') = (col == col' && row /= row') || (col /= col' && row == row')

isQueenMove :: Location -> Location -> Bool
isQueenMove src dest = isRookMove src dest || isBishopMove src dest

isKingMove :: Location -> Location -> Bool
isKingMove (col, row) (col', row') = abs (ord col - ord col') <= 1 && abs (row - row') <= 1
	
getCurrentPlayer :: Board -> Player
getCurrentPlayer (White,_,_) = White 
getCurrentPlayer (Black,_,_) = Black

getWhitePieces :: Board -> [Piece]
getWhitePieces (_,whitePieces,_) = whitePieces

getBlackPieces  :: Board -> [Piece]
getBlackPieces (_,_,blackPieces) = blackPieces 

getLocation :: Piece -> Location
getLocation (P loc) = loc
getLocation (N loc) = loc
getLocation (K loc) = loc
getLocation (R loc) = loc
getLocation (Q loc) = loc
getLocation (B loc) = loc

getPlayer :: Piece -> Board -> Player
getPlayer (P l) (_,whitePieces,blackPieces) = if((P l) `elem` whitePieces) then White else Black
getPlayer (N l) (_,whitePieces,blackPieces) = if((N l) `elem` whitePieces) then White else Black
getPlayer (B l) (_,whitePieces,blackPieces) = if((B l) `elem` whitePieces) then White else Black
getPlayer (R l) (_,whitePieces,blackPieces) = if((R l) `elem` whitePieces) then White else Black
getPlayer (Q l) (_,whitePieces,blackPieces) = if((Q l) `elem` whitePieces) then White else Black
getPlayer (K l) (_,whitePieces,blackPieces) = if((K l) `elem` whitePieces) then White else Black

pieceLocs :: [Piece]->[Location]
pieceLocs pieces=[loc | piece <- pieces, let loc = getLocation piece]


suggestMove :: Piece -> Board -> [Location]
suggestMove piece board = filter (isLegal piece board) allLocations
  where
    allLocations = [(col, row) | col <- ['a'..'h'], row <- [1..8]]
	
move :: Piece -> Location -> Board -> Board
move piece destination board
  | not (isLegal piece board destination) = error "Illegal move for the piece"
  | getCurrentPlayer board /= getPlayer piece board = error "This is not the player's turn"
  | otherwise = (nextPlayer, updatedWhitePieces, updatedBlackPieces)
  where
    nextPlayer = if getCurrentPlayer board == White then Black else White
    (otherPlayerPieces,currentPlayerPieces) = if getCurrentPlayer board == White then (getBlackPieces board, getWhitePieces board) else (getWhitePieces board, getBlackPieces board)
    (capturedPiece, updatedPieces) = capturePiece destination otherPlayerPieces
    updatedPiece = updatePieceLocation piece destination
    updatedWhitePieces = if getCurrentPlayer board == White then replacePiece piece updatedPiece currentPlayerPieces else delete capturedPiece otherPlayerPieces
    updatedBlackPieces = if getCurrentPlayer board == Black then replacePiece piece updatedPiece currentPlayerPieces else delete capturedPiece otherPlayerPieces
    capturePiece loc pieces = case find (\p -> getLocation p == loc) pieces of
      Just captured -> (captured, delete captured pieces)
      Nothing -> (nullPiece, pieces)
    nullPiece = P ('z', 0)
    replacePiece oldPiece newPiece = map (\p -> if p == oldPiece then newPiece else p)
    updatePieceLocation (P _) loc = P loc
    updatePieceLocation (N _) loc = N loc
    updatePieceLocation (B _) loc = B loc
    updatePieceLocation (R _) loc = R loc
    updatePieceLocation (Q _) loc = Q loc
    updatePieceLocation (K _) loc = K loc

