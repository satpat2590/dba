CREATE TABLE IF NOT EXISTS TASKS(
    TID INTEGER PRIMARY KEY AUTOINCREMENT,
    NAME TEXT NOT NULL,
    DESCRIPTION TEXT,
    DUE_DATE INTEGER,            -- When the task must be completed (Unix time)
    STATUS TEXT CHECK(STATUS IN ('NOT_STARTED', 'IN_PROGRESS', 'COMPLETED', 'OVERDUE')) DEFAULT 'NOT_STARTED',
    CATEGORY CHAR(50),
    POINTS INT NOT NULL,
    DIRECTORY TEXT,
    P_TID INT,                    -- Parent task ID for hierarchical tasks
    CREATED_AT INTEGER DEFAULT (strftime('%s', 'now')), -- Current timestamp in Unix time
    FOREIGN KEY (P_TID) REFERENCES TASKS (TID)
);


-- Index for efficient date queries
CREATE INDEX IF NOT EXISTS idx_task_dates ON TASKS(START_DATE, DUE_DATE);
CREATE INDEX IF NOT EXISTS idx_task_status ON TASKS(STATUS);


-- Create TASKTAG table to link tags to tasks
CREATE TABLE IF NOT EXISTS TASKTAG(
    TID INT NOT NULL,
    TAG CHAR(50) NOT NULL,
    PRIMARY KEY (TID, TAG),
    FOREIGN KEY (TID) REFERENCES TASKS (TID)
);

-- Index for faster tag queries
CREATE INDEX IF NOT EXISTS idx_task_tags ON TASKTAG (TAG);

