from sqlalchemy import Column, Integer, String, DateTime
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class Dataset(Base):
    __tablename__ = 'datasets'

    id = Column(Integer, primary_key=True)
    name = Column(String)
    # The legacy DB has other columns like 'depth', 'group_id', etc.
    # We map the essentials for now.
    depth = Column(String)
